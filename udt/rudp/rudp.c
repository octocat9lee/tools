#include "rudp.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

//#define GENERAL_PACKAGE 512
#define GENERAL_PACKAGE 128

struct message
{
    struct message * next;
    uint8_t *buffer;  //store message content
    int sz;
    int cap; //equal to GENERAL_PACKAGE
    int id;
    int tick;
};

struct message_queue
{
    struct message *head;
    struct message *tail;
};

struct array {
    int cap;  //capacity of array
    int n;    //length of array
    int *a;   //content of array
};

struct rudp
{
    struct message_queue send_queue;    // user packages will send
    struct message_queue recv_queue;    // the packages recv
    struct message_queue send_history;  // user packages already send

    struct rudp_package *send_package;  // returns by rudp_update

    struct message *free_list;  // recycle message struct
    struct array send_again;    // package id need send again, 接收端丢失的，发送端需要重新发送的id

    int corrupt;
    int current_tick;
    int last_send_tick;
    int last_expired_tick;
    int send_id;                //每次调用rudp_send加1，内部计数使用
    int recv_id_min;            //每次调用rudp_recv加1，内部计数使用
    int recv_id_max;            //接收端接收到的最大id
    int send_delay;
    int expired;
};

struct rudp *
    rudp_new(int send_delay, int expired_time)
{
    struct rudp * U = malloc(sizeof(*U));
    memset(U, 0, sizeof(*U));
    U->send_delay = send_delay;
    U->expired = expired_time;
    return U;
}

static void
clear_outpackage(struct rudp *U)
{
    struct rudp_package *tmp = U->send_package;
    while(tmp)
    {
        struct rudp_package *next = tmp->next;
        free(tmp);
        tmp = next;
    }
    U->send_package = NULL;
}

static void
free_message_list(struct message *m)
{
    while(m)
    {
        struct message *next = m->next;
        free(m);
        m = next;
    }
}

void
rudp_delete(struct rudp *U)
{
    free_message_list(U->send_queue.head);
    free_message_list(U->recv_queue.head);
    free_message_list(U->send_history.head);
    free_message_list(U->free_list);
    clear_outpackage(U);
    free(U->send_again.a);
}

//创建新的消息报文，如果U中存在可以容纳buffer的空闲块，直接在释放的list中存放新的消息，
//否则创建新的缓冲区，将buffer内容拷贝至message的buffer
static struct message *
new_message(struct rudp *U, const uint8_t *buffer, int sz)
{
    struct message * tmp = U->free_list;
    if(tmp)
    {
        U->free_list = tmp->next;
        if(tmp->cap < sz)
        {
            free(tmp);
            tmp = NULL;
        }
    }
    if(tmp == NULL)
    {
        int cap = sz;
        if(cap < GENERAL_PACKAGE)
        {
            cap = GENERAL_PACKAGE;
        }
        tmp = malloc(sizeof(struct message) + cap);
        tmp->cap = cap;
    }
    tmp->sz = sz;
    tmp->buffer = (uint8_t *)(tmp + 1);
    if(sz > 0 && buffer)
    {
        memcpy(tmp->buffer, buffer, sz);
    }
    tmp->tick = 0;
    tmp->id = 0;
    tmp->next = NULL;
    return tmp;
}

static void
delete_message(struct rudp *U, struct message *m)
{
    m->next = U->free_list;
    U->free_list = m;
}

static void
queue_push(struct message_queue *q, struct message *m)
{
    if(q->tail == NULL)
    {
        q->head = q->tail = m;
    }
    else
    {
        q->tail->next = m;
        q->tail = m;
    }
}

static struct message *
queue_pop(struct message_queue *q, int id)
{
    if(q->head == NULL)
        return NULL;
    struct message *m = q->head;
    if(m->id != id)
        return NULL;
    q->head = m->next;
    m->next = NULL;
    if(q->head == NULL)
        q->tail = NULL;
    return m;
}

//将id按从小到大的顺序插入array中
static void
array_insert(struct array *a, int id)
{
    int i;
    for(i = 0; i < a->n; i++)
    {
        if(a->a[i] == id)
            return;
        if(a->a[i] > id)
        {
            break;
        }
    }
    // insert before i
    if(a->n >= a->cap)
    {
        if(a->cap == 0)
        {
            a->cap = 16;
        }
        else
        {
            a->cap *= 2;
        }
        a->a = realloc(a->a, sizeof(int) * a->cap);
    }
    int j;
    for(j = a->n; j > i; j--)
    {
        a->a[j] = a->a[j - 1];
    }
    a->a[i] = id;
    ++a->n;
}

void
rudp_send(struct rudp *U, const char *buffer, int sz)
{
    assert(sz <= MAX_PACKAGE);
    struct message *m = new_message(U, (const uint8_t *)buffer, sz);
    m->id = U->send_id++;
    m->tick = U->current_tick;
    //将待发送的message加入到U的send_queue中
    queue_push(&U->send_queue, m);
}

int
rudp_recv(struct rudp *U, char buffer[MAX_PACKAGE])
{
    if(U->corrupt)
    {
        U->corrupt = 0;
        return -1;
    }
    struct message *tmp = queue_pop(&U->recv_queue, U->recv_id_min);
    if(tmp == NULL)
    {
        return 0;
    }
    ++U->recv_id_min;
    int sz = tmp->sz;
    if(sz > 0)
    {
        memcpy(buffer, tmp->buffer, sz);
    }
    delete_message(U, tmp);
    return sz;
}

static void
clear_send_expired(struct rudp *U, int tick)
{
    struct message *m = U->send_history.head;
    struct message *last = NULL;
    while(m)
    {
        if(m->tick >= tick)
        {
            break;
        }
        last = m;
        m = m->next;
    }
    if(last)
    {
        // free all the messages before tick
        last->next = U->free_list;
        U->free_list = U->send_history.head;
    }
    U->send_history.head = m;
    if(m == NULL)
    {
        U->send_history.tail = NULL;
    }
}

//每个逻辑包都有一个16bit的序号，从0开始编码，如果超过64K则回到0。
//通讯过程中，如果收到一个数据包和之前的数据包id相差正负32K则做一下更合理的调整。
//例如，如果之前收到的序号为2，而下一个包是FFFF，则认为是2这个序号的前三个，而不是向后一个很远的序号。
//0x8000 = 32K
static int
get_id(struct rudp *U, const uint8_t * buffer)
{
    int id = buffer[0] * 256 + buffer[1];
    id |= U->recv_id_max & ~0xffff;
    if(id < U->recv_id_max - 0x8000)
        id += 0x10000;
    else if(id > U->recv_id_max + 0x8000)
        id -= 0x10000;
    return id;
}

static void
add_request(struct rudp *U, int id)
{
    array_insert(&U->send_again, id);
}

//将id以及buffer生成message并按照id从小到大的顺序插入到U的recv_queue中
static void
insert_message(struct rudp *U, int id, const uint8_t *buffer, int sz)
{
    if(id < U->recv_id_min)
        return;
    if(id > U->recv_id_max || U->recv_queue.head == NULL)
    {
        struct message *m = new_message(U, buffer, sz);
        m->id = id;
        queue_push(&U->recv_queue, m);
        U->recv_id_max = id;
    }
    else
    {
        struct message *m = U->recv_queue.head;
        struct message **last = &U->recv_queue.head;
        do
        {
            if(m->id > id)
            {
                // insert here
                struct message *tmp = new_message(U, buffer, sz);
                tmp->id = id;
                tmp->next = m;
                *last = tmp;
                return;
            }
            last = &m->next;
            m = m->next;
        } while(m);
    }
}

//将发送端告知的TYPE_MISSING数据包id插入到recv_queue接收队列中
static void
add_missing(struct rudp *U, int id)
{
    insert_message(U, id, NULL, -1);
}

#define TYPE_IGNORE 0
#define TYPE_CORRUPT 1
#define TYPE_REQUEST 2
#define TYPE_MISSING 3
#define TYPE_NORMAL 4

//-0 心跳包
//-1 连接异常
//-2 请求包(+2 id)
//-3 异常包(+2 id)
//-4 普通数据

//心跳包和连接异常包仅包含一个字节的tag标识，没有任何的数据
//请求包和异常包包含一个字节的tag标识和2个字节的id
//普通数据将tag编码为(数据长度 + 4)，然后跟2个字节的id，最后填充真实的数据

//tag采用1或2字节编码。如果tag<127编码为1字节，tag是128到32K间时，编码为2字节；其中第一字节高位为1。tag不能超过32K。

//发送端使用extract_package(U, NULL, 0);调用相当于空函数
//接收端使用extract_package(U, r, sizeof(r));解析接收到的数据内容
static void
extract_package(struct rudp *U, const uint8_t *buffer, int sz)
{
    while(sz > 0)
    {
        int len = buffer[0];
        if(len > 127)
        {
            if(sz <= 1)
            {
                U->corrupt = 1;
                return;
            }
            len = (len * 256 + buffer[1]) & 0x7fff;
            buffer += 2;
            sz -= 2;
        }
        else
        {
            buffer += 1;
            sz -= 1;
        }
        switch(len)
        {
            case TYPE_IGNORE:
                if(U->send_again.n == 0)
                {
                    // request next package id
                    array_insert(&U->send_again, U->recv_id_min);
                }
                break;
            case TYPE_CORRUPT:
                U->corrupt = 1;
                return;
            case TYPE_REQUEST:
            case TYPE_MISSING:
                if(sz < 2)
                {
                    U->corrupt = 1;
                    return;
                }
                (len == TYPE_REQUEST ? add_request : add_missing)(U, get_id(U, buffer));
                buffer += 2;
                sz -= 2;
                break;
            default:
                len -= TYPE_NORMAL;
                if(sz < len + 2)
                {
                    U->corrupt = 1;
                    return;
                }
                else
                {
                    int id = get_id(U, buffer);
                    insert_message(U, id, buffer + 2, len);
                }
                buffer += len + 2;
                sz -= len + 2;
                break;
        }
    }
}

struct tmp_buffer
{
    uint8_t buf[GENERAL_PACKAGE];
    int sz;
    struct rudp_package *head;
    struct rudp_package *tail;
};

static void
new_package(struct tmp_buffer *tmp)
{
    struct rudp_package * p = malloc(sizeof(*p) + tmp->sz);
    p->next = NULL;
    p->buffer = (char *)(p + 1);
    p->sz = tmp->sz;
    memcpy(p->buffer, tmp->buf, tmp->sz);
    if(tmp->tail == NULL)
    {
        tmp->head = tmp->tail = p;
    }
    else
    {
        tmp->tail->next = p;
        tmp->tail = p;
    }
    tmp->sz = 0;
}

static int
fill_header(uint8_t *buf, int len, int id)
{
    int sz;
    if(len < 128)
    {
        buf[0] = len;
        ++buf;
        sz = 1;
    }
    else
    {
        //当len长度超过127时，使用2字节编码；并且第1字节高位为1
        buf[0] = ((len & 0x7f00) >> 8) | 0x80;
        buf[1] = len & 0xff;
        buf += 2;
        sz = 2;
    }
    buf[0] = (id & 0xff00) >> 8;
    buf[1] = id & 0xff;
    return sz + 2;
}

static void
pack_request(struct rudp *U, struct tmp_buffer *tmp, int id, int tag)
{
    int sz = GENERAL_PACKAGE - tmp->sz;
    if(sz < 3)
    {
        new_package(tmp);
    }
    uint8_t * buffer = tmp->buf + tmp->sz;
    tmp->sz += fill_header(buffer, tag, id);
}

static void
pack_message(struct tmp_buffer *tmp, struct message *m)
{
    int sz = GENERAL_PACKAGE - tmp->sz;
    if(m->sz > GENERAL_PACKAGE - 4)
    {
        if(tmp->sz > 0)
            new_package(tmp);
        // big package
        sz = 4 + m->sz;
        struct rudp_package * p = malloc(sizeof(*p) + sz);
        p->next = NULL;
        p->buffer = (char *)(p + 1);
        p->sz = sz;
        fill_header((uint8_t *)p->buffer, m->sz + TYPE_NORMAL, m->id);
        memcpy(p->buffer + 4, m->buffer, m->sz);
        if(tmp->tail == NULL)
        {
            tmp->head = tmp->tail = p;
        }
        else
        {
            tmp->tail->next = p;
            tmp->tail = p;
        }
        return;
    }
    if(sz < 4 + m->sz)
    {
        new_package(tmp);
    }
    uint8_t * buf = tmp->buf + tmp->sz;
    int len = fill_header(buf, m->sz + TYPE_NORMAL, m->id);
    tmp->sz += len + m->sz;
    buf += len;
    memcpy(buf, m->buffer, m->sz);
}

//查找丢包id，并将对应的id按照tag+id格式填充到tmp中的buffer中
static void
request_missing(struct rudp *U, struct tmp_buffer *tmp)
{
    int id = U->recv_id_min;
    struct message *m = U->recv_queue.head;
    while(m)
    {
        assert(m->id >= id);
        if(m->id > id)
        {
            int i;
            for(i = id; i < m->id; i++)
            {
                pack_request(U, tmp, i, TYPE_REQUEST);
            }
        }
        id = m->id + 1;
        m = m->next;
    }
}


//根据send_again中的id发送接收端丢失了需要重传的数据包
static void
reply_request(struct rudp *U, struct tmp_buffer *tmp)
{
    int i;
    struct message *history = U->send_history.head;
    for(i = 0; i < U->send_again.n; i++)
    {
        int id = U->send_again.a[i];
        if(id < U->recv_id_min)
        {
            // alreay recv, ignore
            continue;
        }
        for(;;)
        {
            if(history == NULL || id < history->id)
            {
                // expired
                // 发送端得知该id数据包已经过期，通告对方TYPE_MISSING的异常
                pack_request(U, tmp, id, TYPE_MISSING);
                break;
            }
            else if(id == history->id)
            {
                //打包接收端丢失了需要重传的id数据包
                pack_message(tmp, history);
                break;
            }
            history = history->next;
        }
    }

    U->send_again.n = 0;
}

//将send_queue发送队列中的消息打包到tmp的buffer中
static void
send_message(struct rudp *U, struct tmp_buffer *tmp)
{
    struct message *m = U->send_queue.head;
    while(m)
    {
        pack_message(tmp, m);
        m = m->next;
    }
    //将已经发送的数据加入到send_history中
    if(U->send_queue.head)
    {
        if(U->send_history.tail == NULL)
        {
            U->send_history = U->send_queue;
        }
        else
        {
            U->send_history.tail->next = U->send_queue.head;
            U->send_history.tail = U->send_queue.tail;
        }
        U->send_queue.head = NULL;
        U->send_queue.tail = NULL;
    }
}


/*
    1. request missing ( lookup U->recv_queue )
    2. reply request ( U->send_again )
    3. send message ( U->send_queue )
    4. send heartbeat
*/
static struct rudp_package *
gen_outpackage(struct rudp *U)
{
    struct tmp_buffer tmp;
    tmp.sz = 0;
    tmp.head = NULL;
    tmp.tail = NULL;

    request_missing(U, &tmp);
    reply_request(U, &tmp);
    send_message(U, &tmp);

    //至此，tmp中的buffer缓冲区包括本端需要重传的id消息，接收端丢失的需要重传的数据包以及新增发送的数据
    //close tmp
    if(tmp.head == NULL)
    {
        if(tmp.sz == 0)
        {
            tmp.buf[0] = TYPE_IGNORE;
            tmp.sz = 1;
        }
    }
    new_package(&tmp);
    return tmp.head;
}

struct rudp_package *
    rudp_update(struct rudp *U, const void * buffer, int sz, int tick)
{
    U->current_tick += tick;
    //释放已经发送的数据包
    clear_outpackage(U);
    extract_package(U, buffer, sz);
    if(U->current_tick >= U->last_expired_tick + U->expired)
    {
        clear_send_expired(U, U->last_expired_tick);
        U->last_expired_tick = U->current_tick;
    }
    if(U->current_tick >= U->last_send_tick + U->send_delay)
    {
        U->send_package = gen_outpackage(U);
        U->last_send_tick = U->current_tick;
        return U->send_package;
    }
    else
    {
        return NULL;
    }
}
