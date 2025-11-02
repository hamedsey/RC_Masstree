// TO COMPILE
//TO RUN
//./RCC_Client -d mlx5_0 -s 20 -r 20 -g 3 -l 1000000 192.168.1.5 -z 5 -n 8 -t 1
//./RCC_Server -d mlx5_0 -s 20 -r 20 -g 4 -n 16 -t 1

//./RCC_Client -d mlx5_0 -s 20 -r 20 -g 3 -l 30000000 192.168.1.5 -z 5 -m 0 -n 12 -t 12
//./RCC_Server -d mlx5_0 -s 20 -r 20 -g 4 -n 12 -t 12

//gives 16M with no server service time
//./RCC_Server -d mlx5_0 -s 20 -r 200 -g 4 -n 12 -t 12
//./RCC_Client -d mlx5_0 -s 20 -r 200 -g 3 -l 40000000 192.168.1.5 -z 5 -m 0 -n 12 -t 12

//works up to 4092 queues with 12 threads - don't go beyond 14000000
//./RCC_Client -d mlx5_0 -s 20 -r 20 -g 3 -l 14000000 192.168.1.5 -z 5 -m 0 -n 12 -t 12
//./RCC_Server -d mlx5_0 -s 20 -r 20 -g 4 -n 12 -t 12
/*
 * Copyright (c) 2005 Topspin Communications.  All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
//#define _GNU_SOURCE
//#include <config.h>


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <math.h>
#include <fcntl.h>
#include "kvstats.hh"
#include "kvio.hh"
#include "json.hh"
#include "kvtest.hh"
#include "mtclient.hh"
#include "kvrandom.hh"
#include "clp.h"

#include <infiniband/verbs.h>
#include <linux/types.h>  //for __be32 type
#include <sys/mman.h>



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <malloc.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <time.h>
#include <assert.h>
#include <math.h>
#include <pthread.h> 
#include <sched.h>
#include <array>
#include <set>

//#include "pingpong.h"
#include <infiniband/verbs.h>
#include <linux/types.h>  //for __be32 type

#include "rdma_uc.cc"

#include <algorithm>
#include <random>
#include <iostream>
#include <chrono>

#define SERVICE_TIME_SIZE 0x8000

#define MAX_QUEUES 1024
#define SHARED_CQ 1
#define RC 1

#define MEASURE_LATENCIES_ON_CLIENT 1
#define RUNTIME 1
const uint8_t numEpochs = 30;

#define ENABLE_KERNEL 0

//16896*1024 B /8 = 2162688 key-value pairs fit in cache 
//2162688/12 = 180224 key-value pairs per thread
//RO: GETs only
//RW: GETs and PUTs
//for variance, make dataset smaller/larger

enum {
	FIXED = 0,
	NORMAL = 1,
	UNIFORM = 2,
	EXPONENTIAL = 3,
    BIMODAL = 4,
};

enum {
	FB =  5,
	PC = 6,
	SQ = 7,
	NC = 8,
	EXP = 9,
	PCR = 10,
	RR = 11,
	TWENTYP = 12,
	TENP = 13,
	CONTWENTYP = 14,
	CONTENP = 15,
	FIFTYP = 16,
	CONFIFTYP = 17,
	MIXED = 18,
	FIFTYPRAND = 19,
	TWENTYPRAND = 20,
	TENPRAND = 21,
	HUNDREDPRAND = 22,
	MIXEDRAND = 23
};

pthread_barrier_t barrier; 
pthread_barrierattr_t attr;
int ret; 

struct ibv_device      **dev_list;
struct ibv_device	*ib_dev;
char                    *ib_devname = NULL;
char                    *servername = NULL;
unsigned int             port = 18512;
int                      ib_port = 1;
unsigned int             size = 35; //max with RC can be 38 for 3 flits
enum ibv_mtu		 	 mtu = IBV_MTU_1024;
uint64_t             rx_depth = 8192;
uint64_t             window_per_thread = 100;

//#define NUM_KV_PAIRS_PER_CONN 12000

/*
uint64_t             iters = 1024000;
uint64_t			 keysPerConnection = 12000;
*/

//each thread get       1024*12000/12   = NUM_KV_PAIRS_PER_CONN*MAX_QUEUES/NUM_THREADS = 1024000
//each connection gets	1024*12000/1024 = NUM_KV_PAIRS_PER_CONN*MAX_QUEUES/MAX_QUEUES  = 12000

uint64_t             iters = 10240000;
uint64_t			 keysPerConnection = 120000;
//120000*1024*20/(1024*1024*1024) = 2.3 GB

//CALCULATIONS ...
//NUM_KV_PAIRS_PER_CONN*MAX_QUEUES = 1200000*1024 = 1228800000 -> in hex = 0x493E0000
//1228800000 * 8B/ (1024*1024*1024) = 9.1 GB =~ 10GB

//100%
//2.3 GB * 1024 * (1.0)/16.5 MB = 142
//120000 keys per connection / 142 =  845 KV pairs per connection

//20%
///2.3 GB * 1024 * (0.2)/16.5 MB = 28.54
//120000 keys per connection / 28.54 = 4204 KV pairs per connection

//10%
///2.3 GB * 1024 * (0.1)/16.5 MB = 14.27
//120000 keys per connection / = 8409 KV pairs per connection
//since only 10% of connections are active here a larger footprint of these connections 
//is used compared to 100% active connections. but total footprint = LLC size
uint64_t connKeyArraySize = keysPerConnection;


int                      num_cq_events = 0;
int                      sl = 0;
int			 gidx = -1;
char			 gid[33];

uint64_t numConnections = 48;

uint32_t * all_rcnts;
uint32_t * all_scnts;
uint64_t **countPriority;
uint64_t **countSendPriority;
double *rps;
uint64_t goalLoad = 0;
uint64_t numThreads = 0;

uint64_t mean = 1000;
uint64_t bimodal_ratio = 0;
uint64_t long_query_percent = 0;
uint64_t scan_query_numpairs = 0;
//put (1x), get (1x), scan2 (2x), scan16 (5x), scan40 (10x), scan64 (15x), scan95 (20x), scan115 (25x)
//service_times = [1,1,2,5,10,15,20]
//array in hardware = [0,1,1,2,5,10,15,20]
uint8_t scanReqIDs[5] = {3,4,5,6,7};
uint8_t scanNumPairs[5]= {3,18,46,73,105};
double discreteDist[7]= {0,0,0,0,0,0,0};
int udpflag = 1;

uint32_t **random_keys;
uint8_t **op_type;

enum {
	PINGPONG_RECV_WRID = 1,
	PINGPONG_SEND_WRID = 2,
};

static int page_size;


struct pingpong_dest {
	int lid;
	int qpn;
	int psn;
	union ibv_gid gid;
	uint64_t addr;   // buffer address
    uint32_t rkey;   // remote key
};

struct pingpong_context {
	struct ibv_context	*context;
	struct ibv_comp_channel *channel;
	struct ibv_pd		*pd;
	struct ibv_mr		*mr_send;
	struct ibv_mr		**mr_recv;
	struct ibv_cq		*cq;
	struct ibv_qp		*qp;
	char				*buf_send;
	char				**buf_recv;
	int			 size;
	int			 send_flags;
	uint64_t			 rx_depth;
	int			 pending;
	struct ibv_port_attr     portinfo;
	int                      routs;
	struct pingpong_dest     my_dest;
	struct pingpong_dest    *rem_dest;
	uint64_t id;
	//double rps;
	unsigned int rcnt = 0;
	unsigned int scnt = 0;
	unsigned int souts = 0;
	    
	struct ibv_mr *mr_write;                  // MR handle for buf
    volatile char *buf_write;                 // memory buffer pointer, used for
};
struct ibv_context	*globalcontext;

struct pingpong_context **ctx;

struct thread_data {
	struct pingpong_context **ctxs;
	uint64_t thread_id;

	#if MEASURE_LATENCIES_ON_CLIENT
	uint64_t** latencyArr;
	uint64_t* receives;
	uint64_t** latencyArrTimestamp;
	
	uint8_t **whichcore;
	uint64_t rcnt, scnt, souts;
	#endif
};

#define MAXWINDOW 512
struct child {
    int s;
    int udp; // 1 -> udp, 0 -> tcp
    KVConn *conn;

    //struct async a[MAXWINDOW];

    unsigned seq0_;
    unsigned seq1_;
    unsigned long long nsent_;
    int childno;

    inline void check_flush();
};


uint64_t distribution_mode = 0;
uint64_t priority_distribution_mode = 0;

uint64_t connPerThread = 0;
uint8_t *expectedSeqNum;


#if MEASURE_LATENCIES_ON_CLIENT
uint64_t ***latencies;
uint64_t ***latenciesTimestamp;

uint64_t **recvDistribution;
uint8_t ***whichcore;

#endif

#if SHARED_CQ
struct ibv_cq **sharedCQ;
#endif

char* output_dir;

std::set<uint16_t> uniqueNumbers50P[numEpochs];
std::set<uint16_t> uniqueNumbers20P[numEpochs];
std::set<uint16_t> uniqueNumbers10P[numEpochs];
std::set<uint16_t> uniqueNumbers100P[numEpochs];

//uint64_t numExpectedRequestsPerThread;
uint64_t numExpectedRequestsPerConnection;

uint64_t *rcnt;
uint64_t *scnt; 
uint64_t *souts;
uint64_t *largestCoreDepthDifference;

#if ENABLE_KERNEL
uint64_t **connArray;
#endif


void KVConn::hard_check(int tryhard) {
    masstree_precondition(inbufpos_ == inbuflen_);
    if (parser_.empty()) {
        inbufpos_ = inbuflen_ = 0;
        for (auto x : oldinbuf_)
            delete[] x;
        oldinbuf_.clear();
    } else if (inbufpos_ == inbufsz) {
        oldinbuf_.push_back(inbuf_);
        inbuf_ = new char[inbufsz];
        inbufpos_ = inbuflen_ = 0;
    }
    if (tryhard == 1) {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(infd_, &rfds);
        struct timeval tv = {0, 0};
        if (select(infd_ + 1, &rfds, NULL, NULL, &tv) <= 0)
            return;
    } else
        kvflush(out_);

    //if(pid == 0) printf("reading from infd from hard_check \n");
    ssize_t r = read(infd_, inbuf_ + inbufpos_, inbufsz - inbufpos_);
    if (r != -1)
        inbuflen_ += r;
}

uint32_t generateKey(struct pingpong_context *ctx, uint16_t thread_num) {
    static __uint64_t g_lehmer32_state = 0x60bee2bee120fc15;
	g_lehmer32_state *= 0xe4dd58b5;
	
	//Uniform random across all KV pairs
	uint32_t k = (g_lehmer32_state >> (32-2*thread_num)) % (iters*(numThreads));
    return k;
}

/*
uint64_t gen_latency(int mean, int mode, int isMeasThread, uint64_t *serviceTime) {
    
	if(isMeasThread == 1) return mean;
	//else if (mode == FIXED) return mean;
	else {
		static uint64_t index = 0;
		uint64_t result = serviceTime[index];
		//printf("index = %lu, OR  = %lu \n",index,SERVICE_TIME_SIZE | index);
		//if( (SERVICE_TIME_SIZE | index) == 0xFFFF) {
		if(index == SERVICE_TIME_SIZE - 1) {
			//printf("worked!, index = %lu \n",index);
			index = 0;
		}
		else {
			//printf("index = %lu \n",index);
			index++;
		}

		//if(result != 1000) printf("index = %lu, wrong value = %d \n", index, result);
		#if MEAS_GEN_LAT 
			printf("gen lat = %d \n",result); 
		#endif
		//if (result < 0) result = 0;
		return result;
	}
}
*/

inline uint16_t gen_priority(uint16_t *priorityID) {

	static uint16_t index = 0;
	uint16_t result = priorityID[index];
	//printf("index = %lu, OR  = %lu \n",index,SERVICE_TIME_SIZE | index);
	if(index == SERVICE_TIME_SIZE - 1) {
		//printf("worked!, index = %lu \n",index);
		index = 0;
	}
	else {
		//printf("index = %lu \n",index);
		index++;
	}

	/*
	uint64_t * countofPriority = (uint64_t*)malloc(numConnections*(sizeof(uint64_t)));
	for(int o = 0; o < SERVICE_TIME_SIZE; o++) countofPriority[priorityID[o]]++;

	for(int o = 0; o < numConnections; o++) printf("%llu  ", countofPriority[o]);
	printf("\n");
	*/

	return result;
}

/*
uint64_t gen_arrival_time(uint64_t *arrivalSleepTime) {
    
	static uint16_t index = 0;
	uint64_t result = arrivalSleepTime[index];
	//printf("index = %lu, OR  = %lu \n",index,SERVICE_TIME_SIZE | index);
	//if( (SERVICE_TIME_SIZE | index) == 0xFFFF) {
	if(index == SERVICE_TIME_SIZE - 1) {
		//printf("worked!, index = %lu \n",index);
		index = 0;
	}
	else {
		//printf("index = %lu \n",index);
		index++;
	}
	return result;
}
*/

double ran_expo(double lambda){
    double u;
    u = rand() / (RAND_MAX + 1.0);
    return -log(1- u) / lambda;
}

int pp_get_port_info(struct ibv_context *context, int port,
		     struct ibv_port_attr *attr)
{
	return ibv_query_port(context, port, attr);
}

void wire_gid_to_gid(const char *wgid, union ibv_gid *gid)
{
	char tmp[9];
	__be32 v32;
	int i;
	uint32_t tmp_gid[4];

	for (tmp[8] = 0, i = 0; i < 4; ++i) {
		memcpy(tmp, wgid + i * 8, 8);
		sscanf(tmp, "%x", &v32);
		tmp_gid[i] = be32toh(v32);
	}
	memcpy(gid, tmp_gid, sizeof(*gid));
}

void gid_to_wire_gid(const union ibv_gid *gid, char wgid[])
{
	uint32_t tmp_gid[4];
	int i;

	memcpy(tmp_gid, gid, sizeof(tmp_gid));
	for (i = 0; i < 4; ++i)
		sprintf(&wgid[i * 8], "%08x", htobe32(tmp_gid[i]));
}

inline void my_sleep(uint64_t n) {
	//printf("mysleep = %llu \n",n);
	struct timespec ttime,curtime;
	clock_gettime(CLOCK_MONOTONIC,&ttime);
	
	while(1){
		clock_gettime(CLOCK_MONOTONIC,&curtime);
		uint64_t elapsed = (curtime.tv_sec-ttime.tv_sec )/1e-9 + (curtime.tv_nsec-ttime.tv_nsec);

		if (elapsed >= n) {
			//printf("elapsed = %llu \n",elapsed);
			break;
		}
	}
}

static int pp_connect_ctx(struct pingpong_context *ctx, int port, int my_psn,
			  enum ibv_mtu mtu, int sl,
			  struct pingpong_dest *dest, int sgid_idx)
{
	struct ibv_qp_attr attr;// = {
	memset(&attr, 0, sizeof(attr));

	attr.qp_state		= IBV_QPS_RTR;
	attr.path_mtu		= mtu;
	attr.dest_qp_num		= dest->qpn;
	attr.rq_psn		= dest->psn;
	#if RC
		attr.max_dest_rd_atomic = 1;
		attr.min_rnr_timer      = 0;
	#endif
	attr.ah_attr.is_global	= 0;
	attr.ah_attr.dlid		= dest->lid;
	attr.ah_attr.sl		= sl;
	attr.ah_attr.src_path_bits	= 0;
	attr.ah_attr.port_num	= port;

	if (dest->gid.global.interface_id) {
		attr.ah_attr.is_global = 1;
		attr.ah_attr.grh.hop_limit = 1;
		attr.ah_attr.grh.dgid = dest->gid;
		attr.ah_attr.grh.sgid_index = sgid_idx;
	}
	if (ibv_modify_qp(ctx->qp, &attr,
			#if RC
			  IBV_QP_MAX_DEST_RD_ATOMIC |
			  IBV_QP_MIN_RNR_TIMER 		|
			#endif
			  IBV_QP_STATE              |
			  IBV_QP_AV                 |
			  IBV_QP_PATH_MTU           |
			  IBV_QP_DEST_QPN           |
			  IBV_QP_RQ_PSN				
			  )) {
		fprintf(stderr, "Failed to modify QP to RTR\n");
		return 1;
	}

	attr.qp_state	= IBV_QPS_RTS;
	#if RC
		attr.timeout    = 1;
		attr.retry_cnt  = 7;
		attr.rnr_retry  = 7;
		attr.max_rd_atomic  = 1;
	#endif
	attr.sq_psn	    = my_psn;

	if (ibv_modify_qp(ctx->qp, &attr,
			#if RC
			  IBV_QP_TIMEOUT            |
			  IBV_QP_RETRY_CNT          |
			  IBV_QP_RNR_RETRY          |
			  IBV_QP_MAX_QP_RD_ATOMIC   |
			#endif
  			  IBV_QP_STATE                |
			  IBV_QP_SQ_PSN				  
			  )) {
		fprintf(stderr, "Failed to modify QP to RTS\n");
		return 1;
	}

	return 0;
}

static struct pingpong_dest *pp_client_exch_dest(const char *servername, int port, const struct pingpong_dest *my_dest)
{
	struct addrinfo *res, *t;
	struct addrinfo hints;// = {
	memset(&hints, 0, sizeof hints);
	hints.ai_flags    = AI_PASSIVE;
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	char *service;
	char msg[sizeof "0000:000000:000000:0000000000000000:00000000:00000000000000000000000000000000"];
	int n;
	int sockfd = -1;
	struct pingpong_dest *rem_dest = NULL;
	//memset(rem_dest, 0, sizeof(struct pingpong_dest));


	char gid[33];

	if (asprintf(&service, "%d", port) < 0)
		return NULL;

	n = getaddrinfo(servername, service, &hints, &res);

	if (n < 0) {
		fprintf(stderr, "%s for %s:%d\n", gai_strerror(n), servername, port);
		free(service);
		return NULL;
	}

	for (t = res; t; t = t->ai_next) {
		sockfd = socket(t->ai_family, t->ai_socktype, t->ai_protocol);
		if (sockfd >= 0) {
			if (!connect(sockfd, t->ai_addr, t->ai_addrlen))
				break;
			close(sockfd);
			sockfd = -1;
		}
	}

	freeaddrinfo(res);
	free(service);

	if (sockfd < 0) {
		fprintf(stderr, "Couldn't connect to %s:%d\n", servername, port);
		return NULL;
	}

	//printf("port number = %llu \n", port);
	gid_to_wire_gid(&my_dest->gid, gid);
	sprintf(msg, "%04x:%06x:%06x:%p:%08x:%s", my_dest->lid, my_dest->qpn,
							my_dest->psn, my_dest->addr, my_dest->rkey, gid);
	if (write(sockfd, msg, sizeof msg) != sizeof msg) {
		fprintf(stderr, "Couldn't send local address\n");
		goto out;
	}

	if (read(sockfd, msg, sizeof msg) != sizeof msg ||
	    write(sockfd, "done", sizeof "done") != sizeof "done") {
		perror("client read/write");
		fprintf(stderr, "Couldn't read/write remote address\n");
		goto out;
	}


	rem_dest = (struct pingpong_dest *)malloc(sizeof *rem_dest);
	if (!rem_dest)
		goto out;

	//printf("msg = %s \n", msg);
	//for(int z = 0; z < n; z++) printf("%x", msg[z]);

	sscanf(msg, "%x:%x:%x:%p:%x:%s", &rem_dest->lid, &rem_dest->qpn,
						&rem_dest->psn, &rem_dest->addr, &rem_dest->rkey, gid);
	
	//printf("sscanf:  LID 0x%04x, QPN 0x%06x, PSN 0x%06x, ADDR %p, RKEY 0x%08x, GID %s \n", rem_dest->lid, rem_dest->qpn, rem_dest->psn, rem_dest->addr, rem_dest->rkey, gid);

	wire_gid_to_gid(gid, &rem_dest->gid);

out:
	close(sockfd);
	return rem_dest;
}

static struct pingpong_dest *pp_server_exch_dest(struct pingpong_context *ctx,
						 int ib_port, enum ibv_mtu mtu,
						 int port, int sl,
						 const struct pingpong_dest *my_dest,
						 int sgid_idx)
{
	struct addrinfo *res, *t;
	struct addrinfo hints;// = {
	memset(&hints, 0, sizeof hints);
	hints.ai_flags    = AI_PASSIVE;
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	char *service;
	char msg[sizeof "0000:000000:000000:0000000000000000:00000000:00000000000000000000000000000000"];
	int n;
	int sockfd = -1, connfd;
	struct pingpong_dest *rem_dest = NULL;
	char gid[33];

	if (asprintf(&service, "%d", port) < 0)
		return NULL;

	n = getaddrinfo(NULL, service, &hints, &res);

	if (n < 0) {
		fprintf(stderr, "%s for port %d\n", gai_strerror(n), port);
		free(service);
		return NULL;
	}

	for (t = res; t; t = t->ai_next) {
		sockfd = socket(t->ai_family, t->ai_socktype, t->ai_protocol);
		if (sockfd >= 0) {
			n = 1;

			setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &n, sizeof n);

			if (!bind(sockfd, t->ai_addr, t->ai_addrlen))
				break;
			close(sockfd);
			sockfd = -1;
		}
	}

	freeaddrinfo(res);
	free(service);

	if (sockfd < 0) {
		fprintf(stderr, "Couldn't listen to port %d\n", port);
		return NULL;
	}

	listen(sockfd, 1);
	connfd = accept(sockfd, NULL, NULL);
	close(sockfd);
	if (connfd < 0) {
		fprintf(stderr, "accept() failed\n");
		return NULL;
	}

	n = read(connfd, msg, sizeof msg);
	if (n != sizeof msg) {
		perror("server read");
		fprintf(stderr, "%d/%d: Couldn't read remote address\n", n, (int) sizeof msg);
		goto out;
	}

	rem_dest = (struct pingpong_dest *)malloc(sizeof *rem_dest);
	if (!rem_dest)
		goto out;

	sscanf(msg, "%x:%x:%x:%p:%x:%s", &rem_dest->lid, &rem_dest->qpn,
							&rem_dest->psn, &rem_dest->addr, &rem_dest->rkey, gid);
	wire_gid_to_gid(gid, &rem_dest->gid);

	if (pp_connect_ctx(ctx, ib_port, my_dest->psn, mtu, sl, rem_dest,
								sgid_idx)) {
		fprintf(stderr, "Couldn't connect to remote QP\n");
		free(rem_dest);
		rem_dest = NULL;
		goto out;
	}


	gid_to_wire_gid(&my_dest->gid, gid);
	sprintf(msg, "%04x:%06x:%06x:%p:%08x:%s", my_dest->lid, my_dest->qpn,
							my_dest->psn, my_dest->addr, my_dest->rkey, gid);
	if (write(connfd, msg, sizeof msg) != sizeof msg ||
	    read(connfd, msg, sizeof msg) != sizeof "done") {
		fprintf(stderr, "Couldn't send/recv local address\n");
		free(rem_dest);
		rem_dest = NULL;
		goto out;
	}

out:
	close(connfd);
	return rem_dest;
}

static struct pingpong_context *pp_init_ctx(struct ibv_device *ib_dev, int size, int rx_depth, int port)
{
	uint64_t totalBytes = 100;
	uint64_t bufOffset = 4096;
	struct pingpong_context *ctx;

	ctx = (pingpong_context*)calloc(1, sizeof *ctx);
	if (!ctx)
		return NULL;

	ctx->size       = size;
	//printf("size = %llu, ctx size = %llu \n", size, ctx->size);
	//ctx->send_flags = IBV_SEND_INLINE;
	ctx->rx_depth   = rx_depth;

	ctx->buf_send = (char*)malloc(size*sizeof(char));
	//ctx->buf = memalign(page_size, size);
	if (!ctx->buf_send) {
		fprintf(stderr, "Couldn't allocate work buf.\n");
		goto clean_ctx;
	}
	/* FIXME memset(ctx->buf, 0, size); */
	memset(ctx->buf_send, 0x00, size);

	ctx->buf_recv = (char**)malloc(ctx->rx_depth*sizeof(char*));
	for(uint64_t z = 0; z < ctx->rx_depth; z++) {
		ctx->buf_recv[z] = (char*)malloc(size*sizeof(char));
		//ctx->buf = memalign(page_size, size);
		if (!ctx->buf_recv[z]) {
			fprintf(stderr, "Couldn't allocate work buf.\n");
			goto clean_ctx;
		}

		/* FIXME memset(ctx->buf, 0, size); */
		memset(ctx->buf_recv[z], 0x00, size);
	}


	static int poo = 0;
	if(poo == 0) {
		globalcontext = ibv_open_device(ib_dev);
		if (!globalcontext) {
			fprintf(stderr, "Couldn't get context for %s\n",
				ibv_get_device_name(ib_dev));
			goto clean_buffer;
		}
	}
	ctx->context = globalcontext;
	ctx->id = poo;

	ctx->channel = NULL;

	ctx->pd = ibv_alloc_pd(ctx->context);
	if (!ctx->pd) {
		fprintf(stderr, "Couldn't allocate PD\n");
		goto clean_comp_channel;
	}

	ctx->mr_send = ibv_reg_mr(ctx->pd, ctx->buf_send, size, IBV_ACCESS_LOCAL_WRITE);
	if (!ctx->mr_send) {
		fprintf(stderr, "Couldn't register MR\n");
		goto clean_pd;
	}

	ctx->mr_recv = (struct ibv_mr**)malloc(ctx->rx_depth*sizeof(struct ibv_mr*));
	for(uint64_t z = 0; z < ctx->rx_depth; z++) {
		ctx->mr_recv[z] = ibv_reg_mr(ctx->pd, ctx->buf_recv[z], size, IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_WRITE);
		if (!ctx->mr_recv[z]) {
			fprintf(stderr, "Couldn't register MR\n");
			goto clean_pd;
		}
	}

	#if SHARED_CQ
		if(ctx->id % connPerThread == 0) {
			//printf("created sharedCQ, connid = %llu \n", ctx->id);
			sharedCQ[ctx->id/(connPerThread)] = ibv_create_cq(ctx->context, (connPerThread)*(2*rx_depth + 1), NULL, ctx->channel, 0);
			if (!sharedCQ[ctx->id/(connPerThread)]) {
				fprintf(stderr, "Couldn't create CQ\n");
				goto clean_cq;
			}
		}
	#else
		ctx->cq = ibv_create_cq(ctx->context, 2*rx_depth + 1, NULL, ctx->channel, 0);
		if (!ctx->cq) {
			fprintf(stderr, "Couldn't create CQ\n");
			goto clean_cq;
		}
	#endif

	assert(posix_memalign((void**)(&ctx->buf_write), bufOffset, totalBytes) == 0);
	//ctx->buf_write = (volatile char *)malloc(totalBytes);

	//printf("buf address %p \n",(ctx->buf_write));
	//ctx->buf_write[0] = 255;
 	//for (uint z = 0; z < totalBytes; z++) if((uint8_t)ctx->buf_write[z] != 0) printf("buf index %d = %d \n",z,(uint8_t)ctx->buf_write[z]); 

    ctx->mr_write = ibv_reg_mr(ctx->pd, (void *)ctx->buf_write, totalBytes, IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_WRITE);
    assert(ctx->mr_write != NULL);

	{
		struct ibv_qp_attr attr;
		struct ibv_qp_init_attr init_attr;// = {
		memset(&attr, 0, sizeof(attr));
		memset(&init_attr, 0, sizeof(init_attr));

		#if SHARED_CQ
			//printf("sharedCQ index = %llu \n",ctx->id/connPerThread);
			init_attr.send_cq = sharedCQ[ctx->id/connPerThread];
			init_attr.recv_cq = sharedCQ[ctx->id/connPerThread];
		#else
			init_attr.send_cq = ctx->cq;
			init_attr.recv_cq = ctx->cq;
		#endif
		init_attr.cap.max_send_wr  = 8192;//rx_depth;	
		init_attr.cap.max_recv_wr  = 8192;//rx_depth;
		init_attr.cap.max_send_sge = 1;
		init_attr.cap.max_recv_sge = 1;
		init_attr.sq_sig_all = 1;
		#if RC
			init_attr.qp_type = IBV_QPT_RC;
		#else 
			init_attr.qp_type = IBV_QPT_UC;
		#endif
		static uint64_t first = 1;
		if(first == 1) {
			first = 0;
			while(1)
			{				
				ctx->qp = ibv_create_qp(ctx->pd, &init_attr);
				if (!ctx->qp)  {
					fprintf(stderr, "Couldn't create QP\n");
					goto clean_cq;
				}
				if(ctx->qp->qp_num %(MAX_QUEUES) == 0) break;
				else ibv_destroy_qp(ctx->qp);
			}
		}
		else {
			ctx->qp = ibv_create_qp(ctx->pd, &init_attr);
				if (!ctx->qp)  {
					fprintf(stderr, "Couldn't create QP\n");
					goto clean_cq;
				}
		}

		ibv_query_qp(ctx->qp, &attr, IBV_QP_CAP, &init_attr);
		if (init_attr.cap.max_inline_data >= size) {
			ctx->send_flags |= IBV_SEND_INLINE;
		}
	}

	{
		struct ibv_qp_attr attr;// = {
		attr.qp_state        = IBV_QPS_INIT;
		attr.pkey_index      = 0;
		attr.port_num        = port;
		attr.qp_access_flags = IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_WRITE;

		if (ibv_modify_qp(ctx->qp, &attr,
				  IBV_QP_STATE              |
				  IBV_QP_PKEY_INDEX         |
				  IBV_QP_PORT               |
				  IBV_QP_ACCESS_FLAGS)) {
			fprintf(stderr, "Failed to modify QP to INIT\n");
			goto clean_qp;
		}
	}
	poo++;

	return ctx;

clean_qp:
	ibv_destroy_qp(ctx->qp);

clean_cq:
	ibv_destroy_cq(ctx->cq);

clean_mr:
	ibv_dereg_mr(ctx->mr_send);

clean_pd:
	ibv_dealloc_pd(ctx->pd);

clean_comp_channel:
	if (ctx->channel)
		ibv_destroy_comp_channel(ctx->channel);

clean_device:
	ibv_close_device(ctx->context);

clean_buffer:
	free(ctx->buf_send);

clean_ctx:
	free(ctx);

	return NULL;
}

static int pp_close_ctx(struct pingpong_context *ctx)
{
	//printf("destroy qp \n");
	if (ibv_destroy_qp(ctx->qp)) {
		fprintf(stderr, "Couldn't destroy QP\n");
		return 1;
	}
	//printf("qp \n");
	if (ibv_destroy_cq(ctx->cq)) {
		fprintf(stderr, "Couldn't destroy CQ\n");
		return 1;
	}
	//printf("cq \n");

	if (ibv_dereg_mr(ctx->mr_send)) {
		fprintf(stderr, "Couldn't deregister MR\n");
		return 1;
	}
	//printf("mrsend \n");

	for(uint64_t z = 0; z < ctx->rx_depth; z++) {
		if (ibv_dereg_mr(ctx->mr_recv[z])) {
			fprintf(stderr, "Couldn't deregister MR\n");
			return 1;
		}
	}

	//printf("mrrecv \n");

	if (ibv_dealloc_pd(ctx->pd)) {
		fprintf(stderr, "Couldn't deallocate PD\n");
		return 1;
	}
	//printf("pd \n");


	if (ctx->channel) {
		if (ibv_destroy_comp_channel(ctx->channel)) {
			fprintf(stderr, "Couldn't destroy completion channel\n");
			return 1;
		}
	}
	//printf("channel \n");

	if (ibv_close_device(ctx->context)) {
		fprintf(stderr, "Couldn't release context\n");
		return 1;
	}
	//printf("context \n");

	free(ctx->buf_send);
	for(uint64_t z = 0; z < ctx->rx_depth; z++) {
		free(ctx->buf_recv[z]);
	}
	//printf("bufs \n");

	free(ctx);
	//printf("ctx \n");

	return 0;
}

static int pp_post_recv(struct pingpong_context *ctx, uint64_t bufIndex)
{
	struct ibv_sge list;
	memset(&list, 0, sizeof(list));

	list.addr	= (uintptr_t) ctx->buf_recv[bufIndex];
	list.length = ctx->size;
	list.lkey	= ctx->mr_recv[bufIndex]->lkey;

	struct ibv_recv_wr wr;
	memset(&wr, 0, sizeof(wr));

	wr.wr_id	  = bufIndex;
	wr.sg_list    = &list;
	wr.num_sge    = 1;
	//};
	struct ibv_recv_wr *bad_wr;
	memset(&bad_wr, 0, sizeof(bad_wr));

	//printf("inside recv \n");
	return ibv_post_recv(ctx->qp, &wr, &bad_wr);
}

static int pp_post_send(struct pingpong_context *ctx, bool signal, uint16_t length)
{
	struct ibv_sge list;// = {
	memset(&list, 0, sizeof(list));

	list.addr	= (uintptr_t) ctx->buf_send;
	/*
	if(length > ctx->size) {
		printf("length = %llu \n", length);
	}
	*/
	assert(length <= ctx->size);
	
	list.length = length;
	list.lkey	= ctx->mr_send->lkey;

	//assert((uint8_t)ctx->buf_send[9] == 0);
	
	struct ibv_send_wr wr;
	memset(&wr, 0, sizeof(wr));

	wr.wr_id	  = ctx->rx_depth;
	wr.sg_list    = &list;
	wr.num_sge    = 1;
	wr.opcode     = IBV_WR_SEND;
	wr.send_flags = ctx->send_flags | IBV_SEND_INLINE;

	if(signal == true) wr.send_flags = ctx->send_flags | IBV_SEND_SIGNALED; //need to have a signaled completion every signalInterval
	else wr.send_flags = ctx->send_flags;

	struct ibv_send_wr *bad_wr;
	memset(&bad_wr, 0, sizeof(bad_wr));

	return ibv_post_send(ctx->qp, &wr, &bad_wr);
}

static int pp_post_write(struct pingpong_context *ctx, bool signal, uint64_t offset)
{
	struct ibv_sge list;// = {
	memset(&list, 0, sizeof(list));

	list.addr	= (uintptr_t)(ctx->buf_write) + offset;
	//printf("buf index %d = %d \n",offset,(uint8_t)ctx->buf_write[offset]); 
	list.length = 1;
	list.lkey	= ctx->mr_write->lkey;

	struct ibv_send_wr wr;
	memset(&wr, 0, sizeof(wr));

	wr.wr_id	  = ctx->rx_depth;
	wr.sg_list    = &list;
	wr.num_sge    = 1;
	wr.opcode     = IBV_WR_RDMA_WRITE;
	//wr.send_flags = ctx->send_flags | IBV_SEND_INLINE;
	wr.next = NULL;

	//if(signal == true) wr.send_flags = ctx->send_flags | IBV_SEND_SIGNALED; //need to have a signaled completion every signalInterval
	wr.send_flags = ctx->send_flags;

	wr.wr.rdma.remote_addr = ctx->rem_dest->addr + offset;
	//printf("remote addr = %x \n", wr.wr.rdma.remote_addr);
	wr.wr.rdma.rkey = ctx->rem_dest->rkey;

	struct ibv_send_wr *bad_wr;
	memset(&bad_wr, 0, sizeof(bad_wr));

	return ibv_post_send(ctx->qp, &wr, &bad_wr);
}

static void usage(const char *argv0)
{
	printf("Usage:\n");
	printf("  %s            start a server and wait for connection\n", argv0);
	printf("  %s <host>     connect to server at <host>\n", argv0);
	printf("\n");
	printf("Options:\n");
	printf("  -p, --port=<port>      listen on/connect to port <port> (default 18515)\n");
	printf("  -d, --ib-dev=<dev>     use IB device <dev> (default first device found)\n");
	printf("  -i, --ib-port=<port>   use port <port> of IB device (default 1)\n");
	printf("  -s, --size=<size>      size of message to exchange (default 4096)\n");
	printf("  -m, --mtu=<size>       path MTU (default 1024)\n");
	printf("  -r, --rx-depth=<dep>   number of receives to post at a time (default 500)\n");
	printf("  -n, --iters=<iters>    number of exchanges (default 1000)\n");
	printf("  -l, --sl=<sl>          service level value\n");
	printf("  -e, --events           sleep on CQ events (default poll)\n");
	printf("  -g, --gid-idx=<gid index> local port gid index\n");
	printf("  -c, --chk              validate received buffer\n");
}

void* threadfunc(void* x) {
	//printf("hello \n");
	struct thread_data *tdata = (struct thread_data *)x;
	struct pingpong_context **ctxs = (struct pingpong_context **)(tdata->ctxs);
	uint64_t thread_id = tdata->thread_id;
	#if MEASURE_LATENCIES_ON_CLIENT
	uint64_t ** latencyArr = tdata->latencyArr;
	uint64_t *receives = tdata->receives;
	uint64_t ** latencyArrTimestamp = tdata->latencyArrTimestamp;
	uint8_t **whichcore = tdata->whichcore;

	#endif

	uint32_t key_offset = thread_id * iters;
	uint32_t key_number = 0;

    struct child c;
    bzero(&c, sizeof(c));
    c.childno = thread_id;
    c.conn = new KVConn(c.s, !udpflag);

	 /////////////////////////////////////////////////////////////////////
	//generate random keys

	//std::random_device rd; // obtain a random number from hardware
	//std::mt19937 gen(rd()); // seed the generator
	//std::uniform_int_distribution<> d(0, conn->iters*(active_thread_num-1)); // define the range

	/*
		PUT      833ns, 745k
		GET      670ns, 758k
		SCAN(3)  1144ns, 619k
		SCAN(18) 3013ns, 281k
		SCAN(46) 6061ns, 150k
		SCAN(73) 8737ns, 106k
		Trimodal 4155ns , 211k
	*/
	

	/*
	static __uint64_t g_lehmer32_state = 0x60bee2bee120fc15;

	for (int jj = 0; jj < iters; jj++) {
		g_lehmer32_state *= 0xe4dd58b5;
		uint32_t k = (g_lehmer32_state >> (32-2*thread_id)) % (iters*(numThreads));
		//conn->random_keys[ii] = d(gen);
		//random_keys[jj] = k;
	}
	*/


	/////////////////////////////////////////////////////////////////////


	uint64_t offset = thread_id % numConnections;
	uint64_t rcnt = tdata->rcnt; 
	uint64_t scnt = tdata->scnt;
	uint64_t souts = tdata->souts;
	//printf("thread id = %llu \n", thread_id);

	//printf("ctx->id = %llu \n", ctxs[thread_id]->id);
	cpu_set_t cpuset;
    CPU_ZERO(&cpuset);       				   //clears the cpuset
    if(thread_id < 12) CPU_SET(thread_id /*+2*//*12*/, &cpuset);  //set CPU 2 on cpuset
    else CPU_SET(thread_id + 12/*+2*//*12*/, &cpuset);  //set CPU 2 on cpuset
	sched_setaffinity(0, sizeof(cpuset), &cpuset);

	uint64_t singleThreadWait = 1000000000/goalLoad;
	uint64_t avg_inter_arr_ns = numThreads*singleThreadWait;

	if(thread_id == 0) printf("avg_inter_arr_ns = %llu \n",avg_inter_arr_ns);

	//uint64_t *arrivalSleepTime = (uint64_t *)malloc(SERVICE_TIME_SIZE*sizeof(uint64_t));
	std::random_device rd{};
	std::mt19937 gen{rd()};
	std::exponential_distribution<double> exp{1/((double)avg_inter_arr_ns)};//-SEND_SERVICE_TIME)};



	uint64_t singleThreadWaitWarmUp = 1000000000/1000000; //1 kRPS for warmup
	uint64_t avg_inter_arr_ns_warmup = numThreads*singleThreadWaitWarmUp;
	if(thread_id == 0) printf("avg_inter_arr_ns for warmup = %llu \n",avg_inter_arr_ns_warmup);

	std::random_device rdwarmup{};
	std::mt19937 genwarmup{rdwarmup()};
	std::exponential_distribution<double> expwarmup{1/((double)avg_inter_arr_ns_warmup)};//-SEND_SERVICE_TIME)};
	//for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
	//	arrivalSleepTime[i] = exp(gen);
		//printf("arrivalSleepTime = %llu \n", arrivalSleepTime[i]);
	//}



	//uint64_t *serviceTime = (uint64_t *)malloc(SERVICE_TIME_SIZE*sizeof(uint64_t));

	uint16_t **priorityID = (uint16_t **)malloc(numEpochs*sizeof(uint16_t*));
	for(uint16_t ep = 0; ep < numEpochs; ep++) priorityID[ep] = (uint16_t *)malloc(SERVICE_TIME_SIZE*sizeof(uint16_t));

	uint16_t *priority50p = (uint16_t *)malloc(SERVICE_TIME_SIZE*sizeof(uint16_t));
	uint16_t *priority20p = (uint16_t *)malloc(SERVICE_TIME_SIZE*sizeof(uint16_t));
	uint16_t *priority10p = (uint16_t *)malloc(SERVICE_TIME_SIZE*sizeof(uint16_t));

	uint16_t *warmUpPriority = (uint16_t *)malloc(SERVICE_TIME_SIZE*sizeof(uint16_t));
	std::random_device rdd{};
	std::mt19937 genn{rdd()};
	std::uniform_int_distribution<uint64_t>* bmm = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, ((thread_id+1)*connPerThread)-1);
	for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
		//printf("result = %d \n",result);
		warmUpPriority[i] =(*bmm)(genn);
	}

	/*
	if(distribution_mode == FIXED) {
		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			serviceTime[i] = mean;
		}
	}
	else if(distribution_mode == EXPONENTIAL) {
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::exponential_distribution<double> exp{1/(double)mean};
		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) serviceTime[i] = exp(gen);
	}
	else if(distribution_mode == BIMODAL) {
		//printf("generating service time with bimodal distribution \n");
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::discrete_distribution<> bm({double(100-long_query_percent), double(long_query_percent)});
		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			uint8_t result = bm(gen);
			//printf("result = %d \n",result);
			if (result == 0) serviceTime[i] = mean;
			else serviceTime[i] = mean*bimodal_ratio;
		}
		//for (int i = 0; i < SERVICE_TIME_SIZE; ++i) printf("%llu ",serviceTime[i]);
	}
	else printf("Invalid service time distribution mode \n");
	*/

	std::uniform_int_distribution<uint64_t>* bm = nullptr;
	if(priority_distribution_mode == FB) {
		std::random_device rd{};
		std::mt19937 gen{rd()};
    	bm = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, ((thread_id+1)*connPerThread)-1);
    	//std::uniform_int_distribution<uint64_t> bm(0, numConnections-1);

		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			priorityID[numEpochs-1][i] =(*bm)(gen);
		}

		/*
		for (int i = 0; i < SERVICE_TIME_SIZE; ++i) {
			printf("%llu ",priorityID[i]);
			if ((i + 1) % 20 == 0) // Print 20 numbers per line for readability
				printf("\n");
		}
		*/
	}
	else if(priority_distribution_mode == PC) {
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::discrete_distribution<> bm({95, 5});
		
		uint16_t numQueuesGettingMoreLoad = /*numConnections*/ connPerThread/5; //8/5;
		uint16_t twentyPercent = thread_id*connPerThread; //0
		uint16_t eightyPercent = twentyPercent+numQueuesGettingMoreLoad;

		std::uniform_int_distribution<uint64_t>* ur20 = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, eightyPercent-1);
		std::uniform_int_distribution<uint64_t>* ur80 = new std::uniform_int_distribution<uint64_t>(eightyPercent, ((thread_id+1)*connPerThread)-1);

		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			if(bm(gen) == 0) {
				priorityID[numEpochs-1][i] = (*ur20)(gen);//twentyPercent;//8*twentyPercent;
				//if(twentyPercent == numQueuesGettingMoreLoad-1) twentyPercent = 0;
				//else twentyPercent++;
			}
			else {
				priorityID[numEpochs-1][i] = (*ur80)(gen);//eightyPercent;//8*eightyPercent;
				//if(eightyPercent == numConnections-1/*7*/) eightyPercent = numQueuesGettingMoreLoad;
				//else eightyPercent++;
			}
		}
	}
	else if(priority_distribution_mode == SQ) {
		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			//priorityID[i] = numConnections-1;
			priorityID[numEpochs-1][i] = thread_id*connPerThread;
		}
	}
	else if(priority_distribution_mode == NC) {
		//traffic passes through 20 queues all the time and through the rest with a probability of 5%
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::discrete_distribution<> bm({95, 5});
		
		uint16_t numQueuesGettingMoreLoad = 20;
		//uint16_t numQueuesGettingLessLoad = numConnections-numQueuesGettingMoreLoad;

		uint16_t twentyPercent = 0;
		uint16_t eightyPercent = numQueuesGettingMoreLoad;

		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			if(bm(gen) == 0) {
				priorityID[numEpochs-1][i] = twentyPercent;
				if(twentyPercent == numQueuesGettingMoreLoad-1) twentyPercent = 0;
				else twentyPercent++;
			}
			else {
				priorityID[numEpochs-1][i] = eightyPercent;
				if(eightyPercent == numConnections-1) eightyPercent = numQueuesGettingMoreLoad;
				else eightyPercent++;
			}
		}
	}
	else if(priority_distribution_mode == EXP) {

		//generating 10000 item array of discrete exponentially distributed priorities
		const uint64_t arrayLength = 10000;
        std::array<uint64_t, arrayLength> arr;
		std::random_device rdo{};
		std::mt19937 geno{rdo()};
		double meanPriorities = numConnections/2;//4;
		//double lambda = 100; // Exponential distribution parameter
		std::exponential_distribution<double> expo{1/(double)meanPriorities};
		for (int i = 0; i < arr.size(); ++i) {
			// Generate a random number from the exponential distribution
			double randomNumber = expo(geno);

			// Scale the random number to the range [0, 255]
			uint64_t scaledNumber = static_cast<uint64_t>(randomNumber) % numConnections;

			arr[i] = (numConnections-1) - scaledNumber;
		}

		// Sort the array in ascending order
		std::sort(arr.begin(), arr.end());
		/*
		uint64_t count = 0;
		for(int i = 0; i < numConnections; i++) {
			count = 0;
			for(int j = 0; j < 10000; j++) {
				if(i == arr[j]) count++;
			}
			printf("i = %llu, count = %llu \n", i , count);
		}

		for (int i = 0; i < arr.size(); ++i) {
			std::cout << arr[i] << " ";
			if ((i + 1) % 20 == 0) // Print 20 numbers per line for readability
				std::cout << std::endl;
		}
		*/
    	std::uniform_int_distribution<uint64_t> bm(0, arrayLength-1);


		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			priorityID[numEpochs-1][i] = arr[bm(gen)];
		}
		/*
		for (int i = 0; i < SERVICE_TIME_SIZE; ++i) {
			printf("%llu ",priorityID[i]);
			if ((i + 1) % 20 == 0) // Print 20 numbers per line for readability
				printf("\n");
		}
		*/
	}
	else if(priority_distribution_mode == PCR) {
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::discrete_distribution<> bm({95, 5});
		
		uint16_t numQueuesGettingMoreLoad = numConnections/5; //8/5;
		uint16_t twentyPercent = 0;
		uint16_t eightyPercent = numQueuesGettingMoreLoad;

		std::uniform_int_distribution<uint64_t>* ur20 = new std::uniform_int_distribution<uint64_t>(0, numQueuesGettingMoreLoad-1);
		std::uniform_int_distribution<uint64_t>* ur80 = new std::uniform_int_distribution<uint64_t>(numQueuesGettingMoreLoad, numConnections-1);

		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			if(bm(gen) == 0) {
				priorityID[numEpochs-1][i] = numConnections-1-(*ur20)(gen);//twentyPercent;//8*twentyPercent;
				//if(twentyPercent == numQueuesGettingMoreLoad-1) twentyPercent = 0;
				//else twentyPercent++;
			}
			else {
				priorityID[numEpochs-1][i] = numConnections-1-(*ur80)(gen);//eightyPercent;//8*eightyPercent;
				//if(eightyPercent == numConnections-1/*7*/) eightyPercent = numQueuesGettingMoreLoad;
				//else eightyPercent++;
			}
		}
	}
	else if(priority_distribution_mode == RR) {
		connPerThread = numConnections/numThreads;
		uint64_t tempConn = thread_id*connPerThread;
		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			priorityID[numEpochs-1][i] = tempConn;
			if(tempConn == ((thread_id+1)*connPerThread) - 1) tempConn = thread_id*connPerThread;
			else tempConn++;
			//priorityID[i] = thread_id*connPerThread;
		}
	}
	else if(priority_distribution_mode == TWENTYP) { //each thread sends to 20% of its fraction of queues
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::discrete_distribution<> bm({95, 5});

		uint16_t numQueuesGettingMoreLoad = /*numConnections*/ connPerThread/5; //8/5;
		uint16_t twentyPercent = thread_id*connPerThread; //0
		uint16_t eightyPercent = twentyPercent+numQueuesGettingMoreLoad;

		std::uniform_int_distribution<uint64_t>* ur20 = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, eightyPercent-1);

		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			priorityID[numEpochs-1][i] = (*ur20)(gen);//twentyPercent;//8*twentyPercent;
		}
	}
	else if(priority_distribution_mode == TENP) { //each thread sends to 10% of its fraction of queues
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::discrete_distribution<> bm({95, 5});

		uint16_t numQueuesGettingMoreLoad = /*numConnections*/ connPerThread/10; //8/5;
		uint16_t twentyPercent = thread_id*connPerThread; //0
		uint16_t eightyPercent = twentyPercent+numQueuesGettingMoreLoad;

		std::uniform_int_distribution<uint64_t>* ur20 = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, eightyPercent-1);

		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			priorityID[numEpochs-1][i] = (*ur20)(gen);//twentyPercent;//8*twentyPercent;
		}
	}
	else if(priority_distribution_mode == CONTWENTYP) {
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::discrete_distribution<> bm({95, 5});

		uint16_t numQueuesGettingMoreLoad = (numConnections/5); //8/5; 200
		connPerThread = numQueuesGettingMoreLoad/numThreads;
		uint16_t twentyPercent = thread_id*connPerThread; //0
		uint16_t eightyPercent = twentyPercent+connPerThread;

		std::uniform_int_distribution<uint64_t>* ur20 = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, eightyPercent-1);

		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			priorityID[numEpochs-1][i] = (*ur20)(gen);//twentyPercent;//8*twentyPercent;
		}
	}
	else if(priority_distribution_mode == CONTENP) {
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::discrete_distribution<> bm({95, 5});

		uint16_t numQueuesGettingMoreLoad = (numConnections/10); //8/5; 200
		connPerThread = numQueuesGettingMoreLoad/numThreads;
		uint16_t twentyPercent = thread_id*connPerThread; //0
		uint16_t eightyPercent = twentyPercent+connPerThread;

		std::uniform_int_distribution<uint64_t>* ur20 = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, eightyPercent-1);

		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			priorityID[numEpochs-1][i] = (*ur20)(gen);//twentyPercent;//8*twentyPercent;
		}
	}
	else if(priority_distribution_mode == FIFTYP) { //each thread sends to 50% of its fraction of queues
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::discrete_distribution<> bm({95, 5});

		uint16_t numQueuesGettingMoreLoad = /*numConnections*/ connPerThread/2; //8/5;
		uint16_t twentyPercent = thread_id*connPerThread; //0
		uint16_t eightyPercent = twentyPercent+numQueuesGettingMoreLoad;

		std::uniform_int_distribution<uint64_t>* ur20 = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, eightyPercent-1);

		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			priorityID[numEpochs-1][i] = (*ur20)(gen);//twentyPercent;//8*twentyPercent;
		}
	}
	else if(priority_distribution_mode == CONFIFTYP) { //each thread sends to an even fraction of the first 50% of queues
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::discrete_distribution<> bm({95, 5});

		uint16_t numQueuesGettingMoreLoad = numConnections/2; //8/5;
		connPerThread = numQueuesGettingMoreLoad/numThreads;
		uint16_t twentyPercent = thread_id*connPerThread; //0
		uint16_t eightyPercent = twentyPercent+connPerThread;

		std::uniform_int_distribution<uint64_t>* ur20 = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, eightyPercent-1);

		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			//printf("result = %d \n",result);
			priorityID[numEpochs-1][i] = (*ur20)(gen);//twentyPercent;//8*twentyPercent;
		}
	}
	else if(priority_distribution_mode == MIXED) {
		std::random_device rd{};
		std::mt19937 gen{rd()};
    	bm = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, ((thread_id+1)*connPerThread)-1);
		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) priorityID[0][i] =(*bm)(gen);


		uint16_t numQueuesGettingMoreLoad = numConnections/2; //8/5;
		connPerThread = numQueuesGettingMoreLoad/numThreads;
		uint16_t twentyPercent = thread_id*connPerThread; //0
		uint16_t eightyPercent = twentyPercent+connPerThread;
		std::uniform_int_distribution<uint64_t>* ur50 = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, eightyPercent-1);
		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) priority50p[i] = (*ur50)(gen);

		numQueuesGettingMoreLoad = (numConnections/5); //8/5; 200
		connPerThread = numQueuesGettingMoreLoad/numThreads;
		twentyPercent = thread_id*connPerThread; //0
		eightyPercent = twentyPercent+connPerThread;
		std::uniform_int_distribution<uint64_t>* ur20 = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, eightyPercent-1);
		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) priority20p[i] = (*ur20)(gen);

		numQueuesGettingMoreLoad = (numConnections/10); //8/5; 200
		connPerThread = numQueuesGettingMoreLoad/numThreads;
		twentyPercent = thread_id*connPerThread; //0
		eightyPercent = twentyPercent+connPerThread;
		std::uniform_int_distribution<uint64_t>* ur10 = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, eightyPercent-1);
		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) priority10p[i] = (*ur10)(gen);

	}
	else if(priority_distribution_mode == FIFTYPRAND) { //each thread sends to 50% of its fraction of queues
		for(uint16_t e = 0; e < numEpochs; e++) {
			uint16_t numConsEachThreadResponsibleFor = (uniqueNumbers50P[e].size())/numThreads;
			//printf("numConsEachThreadResponsibleFor = %llu \n", numConsEachThreadResponsibleFor);
			std::uniform_int_distribution<uint64_t>* ur50 = new std::uniform_int_distribution<uint64_t>(thread_id*numConsEachThreadResponsibleFor, ((thread_id+1)*numConsEachThreadResponsibleFor)-1);

			for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
				std::set<uint16_t>::iterator it = uniqueNumbers50P[e].begin();
				std::advance(it, (*ur50)(gen));
				priorityID[e][i] = *it;//twentyPercent;//8*twentyPercent;
			}
		}
	}
	else if(priority_distribution_mode == TWENTYPRAND) { //each thread sends to 20% of its fraction of queues
		for(uint16_t e = 0; e < numEpochs; e++) {
			uint16_t numConsEachThreadResponsibleFor = (uniqueNumbers20P[e].size())/numThreads;
			//printf("numConsEachThreadResponsibleFor = %llu \n", numConsEachThreadResponsibleFor);
			std::uniform_int_distribution<uint64_t>* ur20 = new std::uniform_int_distribution<uint64_t>(thread_id*numConsEachThreadResponsibleFor, ((thread_id+1)*numConsEachThreadResponsibleFor)-1);

			for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
				std::set<uint16_t>::iterator it = uniqueNumbers20P[e].begin();
				std::advance(it, (*ur20)(gen));
				priorityID[e][i] = *it;//twentyPercent;//8*twentyPercent;
			}
		}
	}
	else if(priority_distribution_mode == TENPRAND) { //each thread sends to 10% of its fraction of queues
		for(uint16_t e = 0; e < numEpochs; e++) {
			uint16_t numConsEachThreadResponsibleFor = (uniqueNumbers10P[e].size())/numThreads;
			//printf("numConsEachThreadResponsibleFor = %llu \n", numConsEachThreadResponsibleFor);
			//std::uniform_int_distribution<uint64_t>* ur10 = new std::uniform_int_distribution<uint64_t>(0, uniqueNumbers10P[e].size()-1);
			std::uniform_int_distribution<uint64_t>* ur10 = new std::uniform_int_distribution<uint64_t>(thread_id*numConsEachThreadResponsibleFor, ((thread_id+1)*numConsEachThreadResponsibleFor)-1);

			for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
				std::set<uint16_t>::iterator it = uniqueNumbers10P[e].begin();
				std::advance(it, (*ur10)(gen));
				priorityID[e][i] = *it;//twentyPercent;//8*twentyPercent;
			}
		}
	}
	else if(priority_distribution_mode == HUNDREDPRAND) { //each thread sends to 10% of its fraction of queues
		for(uint16_t e = 0; e < numEpochs; e++) {

			std::random_device rd{};
			std::mt19937 gen{rd()};
			bm = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, ((thread_id+1)*connPerThread)-1);
			//std::uniform_int_distribution<uint64_t> bm(0, numConnections-1);

			for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
				//printf("result = %d \n",result);
				priorityID[e][i] =(*bm)(gen);
			}
			/*
			uint16_t numConsEachThreadResponsibleFor = (uniqueNumbers100P[e].size())/numThreads;
			//printf("numConsEachThreadResponsibleFor = %llu \n", numConsEachThreadResponsibleFor);
			//std::uniform_int_distribution<uint64_t>* ur100 = new std::uniform_int_distribution<uint64_t>(0, uniqueNumbers100P[e].size()-1);
			std::uniform_int_distribution<uint64_t>* ur100 = new std::uniform_int_distribution<uint64_t>(thread_id*numConsEachThreadResponsibleFor, ((thread_id+1)*numConsEachThreadResponsibleFor)-1);

			for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
				std::set<uint16_t>::iterator it = uniqueNumbers100P[e].begin();
				std::advance(it, (*ur100)(gen));
				priorityID[e][i] = *it;//twentyPercent;//8*twentyPercent;
			}
			*/
		}
	}
	else if(priority_distribution_mode == MIXEDRAND) { //each thread sends to 10% of its fraction of queues
		for(uint16_t e = 0; e < numEpochs; e++) {

			if( (e >= 0 && e < 25) ) {
				
				uint16_t numConsEachThreadResponsibleFor = (uniqueNumbers100P[e].size())/numThreads;
				//printf("numConsEachThreadResponsibleFor = %llu \n", numConsEachThreadResponsibleFor);
				std::uniform_int_distribution<uint64_t>* ur100 = new std::uniform_int_distribution<uint64_t>(thread_id*numConsEachThreadResponsibleFor, ((thread_id+1)*numConsEachThreadResponsibleFor)-1);

				for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
					std::set<uint16_t>::iterator it = uniqueNumbers100P[e].begin();
					std::advance(it, (*ur100)(gen));
					priorityID[e][i] = *it;//twentyPercent;//8*twentyPercent;
				}
				
				/*
				std::random_device rd{};
				std::mt19937 gen{rd()};
				bm = new std::uniform_int_distribution<uint64_t>(thread_id*connPerThread, ((thread_id+1)*connPerThread)-1);
				//std::uniform_int_distribution<uint64_t> bm(0, numConnections-1);

				for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
					//printf("result = %d \n",result);
					priorityID[e][i] =(*bm)(gen);
				}
				*/
			}
			else if( (e >= 25 && e < 50) ) {
				uint16_t numConsEachThreadResponsibleFor = (uniqueNumbers50P[e].size())/numThreads;
				//printf("numConsEachThreadResponsibleFor = %llu \n", numConsEachThreadResponsibleFor);
				std::uniform_int_distribution<uint64_t>* ur50 = new std::uniform_int_distribution<uint64_t>(thread_id*numConsEachThreadResponsibleFor, ((thread_id+1)*numConsEachThreadResponsibleFor)-1);

				for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
					std::set<uint16_t>::iterator it = uniqueNumbers50P[e].begin();
					std::advance(it, (*ur50)(gen));
					priorityID[e][i] = *it;//twentyPercent;//8*twentyPercent;
				}
			}
			else if( (e >= 50 && e < 75) ) {
				uint16_t numConsEachThreadResponsibleFor = (uniqueNumbers20P[e].size())/numThreads;
				//printf("numConsEachThreadResponsibleFor = %llu \n", numConsEachThreadResponsibleFor);
				std::uniform_int_distribution<uint64_t>* ur20 = new std::uniform_int_distribution<uint64_t>(thread_id*numConsEachThreadResponsibleFor, ((thread_id+1)*numConsEachThreadResponsibleFor)-1);

				for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
					std::set<uint16_t>::iterator it = uniqueNumbers20P[e].begin();
					std::advance(it, (*ur20)(gen));
					priorityID[e][i] = *it;//twentyPercent;//8*twentyPercent;
				}
			}
			else {
				uint16_t numConsEachThreadResponsibleFor = (uniqueNumbers10P[e].size())/numThreads;
				//printf("numConsEachThreadResponsibleFor = %llu \n", numConsEachThreadResponsibleFor);
				std::uniform_int_distribution<uint64_t>* ur10 = new std::uniform_int_distribution<uint64_t>(thread_id*numConsEachThreadResponsibleFor, ((thread_id+1)*numConsEachThreadResponsibleFor)-1);

				for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
					std::set<uint16_t>::iterator it = uniqueNumbers10P[e].begin();
					std::advance(it, (*ur10)(gen));
					priorityID[e][i] = *it;//twentyPercent;//8*twentyPercent;
				}
			}
		}
	}

	//reduce logic
	//get rid of send completions
	//poll less items
	
	//uint64_t prio_prev = 0;
	struct pingpong_context *ctx = ctxs[0];
	printf("rx_depth = %llu \n", ctx->rx_depth);

	uint16_t signalInterval = 8192 - ctx->rx_depth;
	printf("signalInterval = %llu \n", signalInterval);

	/*
	uint8_t *expectedSeqNum = (uint8_t*)malloc(numConnections*sizeof(uint8_t));
	for(uint64_t g = 0; g < numConnections; g++) expectedSeqNum[g] = 0;
	
	uint64_t *outOfOrderNum = (uint64_t*)malloc(numConnections*sizeof(uint64_t));
	for(uint64_t g = 0; g < numConnections; g++) outOfOrderNum[g] = 0;
	*/

	//uint8_t *firstOne = (uint8_t*)malloc(numConnections*sizeof(uint8_t));
	//for(uint64_t g = 0; g < numConnections; g++) firstOne[g] = 1;


	uint64_t index = 0;

	/*
	uint64_t * countofPriority = (uint64_t*)malloc(numConnections*(sizeof(uint64_t)));
	for(int o = 0; o < SERVICE_TIME_SIZE; o++) countofPriority[priorityID[o]]++;

	for(int o = 0; o < numConnections; o++) printf("%llu  ", countofPriority[o]);
	printf("\n");
	*/

	uint32_t ingressTS = 0;
	uint32_t egressTS = 0;
	uint64_t latency = 0;
	uint8_t epochNumber = 0;
	uint16_t priority = 0;

	ret = pthread_barrier_wait(&barrier);

	struct timespec ttime,curtime;
	clock_gettime(CLOCK_MONOTONIC,&ttime);

	struct timespec realStart, epochStart;
	
	while(epochNumber-1 != numEpochs) {

		clock_gettime(CLOCK_MONOTONIC,&epochStart);

		while (1) {
				//compare to previous version that was save
				//try adding shared CQ
				//try adding PC traffic
				//go back to doing send completions (didn't work)
				//lowering generated load (works to some extent) - works when capping load at 15000000
			//if(thread_id % 2 == 1) {
			//	sleep(10);
			//	break;
			//}
			//uint16_t priority = priorityID[index];/*(*bm)(gen);*/ //gen_priority(priorityID);
			//if(index == SERVICE_TIME_SIZE - 1) index = 0;
			//else index++;
			//memset(ctx->buf_send+10, 0x00, 50);

			if(priority_distribution_mode == MIXED) {
				if(epochNumber == 0) priority = /*(*bm)(gen);*/ gen_priority(warmUpPriority);
				else if(epochNumber == 1 || epochNumber == 2) priority = /*(*bm)(gen);*/ gen_priority(priorityID[0]);
				else if(epochNumber == 3) priority = /*(*bm)(gen);*/ gen_priority(priority50p);
				else if(epochNumber == 4) priority = /*(*bm)(gen);*/ gen_priority(priority20p);
				else if(epochNumber == 5) priority = /*(*bm)(gen);*/ gen_priority(priority10p);
				else assert(false);
			}
			else if(priority_distribution_mode == FIFTYPRAND || priority_distribution_mode == TWENTYPRAND || priority_distribution_mode == TENPRAND || priority_distribution_mode == HUNDREDPRAND || priority_distribution_mode == MIXEDRAND) {
				if(epochNumber == 0) priority = /*(*bm)(gen);*/ gen_priority(warmUpPriority);
				else priority = /*(*bm)(gen);*/ gen_priority(priorityID[epochNumber-1]);
			}
			else priority = /*(*bm)(gen);*/ gen_priority(priorityID[numEpochs-1]);
			//assert(priority == 0);
			//printf("T %llu , priority = %llu \n",thread_id, priority);
			//if(priority != prio_prev) 
			ctx = ctxs[priority];
			//printf("souts = %llu, rcnt = %llu, ctx->rx_depth = %llu \n", ctx->souts, ctx->rcnt, ctx->rx_depth);
			//if (servername && (ctx->souts - (ctx->scnt*signalInterval) < signalInterval)) {
			//prio_prev = priority;

			if (servername && (ctx->souts - ctx->rcnt < ctx->rx_depth)) { //&& (souts - rcnt < window_per_thread)) {

				//uint64_t req_lat = gen_latency(mean, distribution_mode, 0, serviceTime);
				//req_lat = req_lat >> 4;
				//printf("lat = %d \n",req_lat); 


				#if MEAS_GEN_LAT 
					printf("lat = %d \n",req_lat); 
				#endif
				//uint8_t lat_lower = req_lat & ((1u <<  8) - 1);//req_lat % 0x100;
				//uint8_t lat_upper = (req_lat >> 8) & ((1u <<  8) - 1);//req_lat / 0x100;f
				//printf("sleep_int_lower = %lu, sleep_int_upper = %lu, sleep_time = %lu \n", lat_lower, lat_upper, req_lat);

				//service time
				//if(firstOne[priority] == 1) {
				//	ctx->buf_send[0] = 128; //128, 192
				//	firstOne[priority] = 0;
				//}
				//else 

				if(epochNumber == 0) {
                    ctx->buf_send[0] = 0; //0, 64
                    uint32_t x = key_offset+key_number;
                    key_number++;
					//if(key_number % 10000 == 0) printf("T%llu put - key: %d \n", thread_id, x);
					if(key_number == iters) printf("T%llu - done populating masstree \n", thread_id);
                    quick_istr key(x), value(x+1);
                    c.conn->sendputwhole(key.string(), value.string(), 0);  

                    memcpy(ctx->buf_send+10, c.conn->out_->buf, c.conn->out_->n);
                }
				else {
					//Uniform random across all KV pairs
					//uint32_t k = generateKey(ctx, thread_id);
					//printf("key = %d \n",k);
					
					//Uniform random across a connection's partition of KV pairs
					uint32_t k = random_keys[priority][key_number];
					uint8_t cmdType = op_type[priority][key_number];
					key_number++;
					if(key_number == keysPerConnection) key_number = 0;

					uint8_t reqID = 0;
					switch(cmdType) {
					case 0:
					{
						//PUT
						//printf("put - key: %d \n", k);
						quick_istr key(k), value(k+1);
						c.conn->sendputwhole(key.string(), value.string(), 0);
						reqID = 1;
						break;
					}
					case 1:
					{
						//GET
						//printf("get - key: %d \n", k);
						quick_istr key(k);
						c.conn->sendgetwhole(key.string(), 0);
						reqID = 2;
						break;
					}
					case 2 ... 6:
					{
						//SCAN
						//printf("T%d, SCAN %d \n",thread_num, scanNumPairs[cmdType-2]);
						//printf("scan_key = %d, \n",scan_key);
						//printf("scan - key: %d , numPairs = %d \n", k, scanNumPairs[cmdType-2]);
						quick_istr key(k);
						c.conn->sendscanwhole(key.string(), scanNumPairs[cmdType-2], 1);
						reqID = scanReqIDs[cmdType-2];
						break;
					}
					default: 
						printf("invalid command type %d \n",cmdType);
					}


                    memcpy(ctx->buf_send+10, c.conn->out_->buf, c.conn->out_->n);
                    ctx->buf_send[0] = 16; //0, 64
				}

				//ctx->buf_send[19] = priority;

				//printf("c.conn->out_->n = %llu \n", c.conn->out_->n);
                /*
				for(int p = 0; p < c.conn->out_->n; p++)
				{	
					printf("%x",(c.conn->out_->buf[p]));
				}
				printf("\n");
                */
                /*
				for(int p = 0; p < c.conn->out_->n; p++)
				{	
					printf("%x",(ctx->buf_send[p+10]));
				}
				printf("\n");
				*/

				uint64_t success = pp_post_send(ctx, (ctx->souts%signalInterval == 0), c.conn->out_->n+10);
				if (success == EINVAL) printf("Invalid value provided in wr \n");
				else if (success == ENOMEM)	printf("Send Queue is full or not enough resources to complete this operation \n");
				else if (success == EFAULT) printf("Invalid value provided in qp \n");
				else if (success != 0) {
					printf("success = %d, \n",success);
					fprintf(stderr, "Couldn't post send 3 \n");
					//return 1;
				}
				else {
					c.conn->out_->n = 0;
					ctx->souts++;
					souts++;
					//printf("send posted ... \n");
				}

				//success = pp_post_write(ctx, (ctx->souts%signalInterval == 0), 0);
				/*
				if(ctx->souts == 1) {
					//if(ctx->souts % 2 == 0) 
					(res.buf)[0] = 255;
					//else (res.buf)[0] = 255;
					success = post_send(&res, IBV_WR_RDMA_WRITE, priority);
					if (success == EINVAL) printf("Invalid value provided in wr \n");
					else if (success == ENOMEM)	printf("Send Queue is full or not enough resources to complete this operation \n");
					else if (success == EFAULT) printf("Invalid value provided in qp \n");
					else if (success != 0) {
						printf("success = %d, \n",success);
						fprintf(stderr, "Couldn't post send 3 \n");
						//return 1;
					}
					else {
						//printf("write posted ... \n");
					}
				}
				*/
				countSendPriority[thread_id][priority]++;
				//double interArrivalWaitTime = ran_expo(1/1000);

				/*
				if(priority_distribution_mode == MIXED) {
					if(epochNumber > 0) {
						uint64_t arrival_wait_time = exp(gen);//gen_arrival_time(arrivalSleepTime);
						//printf("%llu \n", arrival_wait_time);
						clock_gettime(CLOCK_MONOTONIC,&curtime);
						uint64_t elapsed = (curtime.tv_sec-ttime.tv_sec )/1e-9 + (curtime.tv_nsec-ttime.tv_nsec);
						if(elapsed < arrival_wait_time) my_sleep(arrival_wait_time - elapsed);
						//my_sleep(arrival_wait_time);
					}
				}
				else {
				*/
					if(epochNumber > 0) {
						uint64_t arrival_wait_time = exp(gen);//gen_arrival_time(arrivalSleepTime);
						//printf("%llu \n", arrival_wait_time);
						clock_gettime(CLOCK_MONOTONIC,&curtime);
						uint64_t elapsed = (curtime.tv_sec-ttime.tv_sec )/1e-9 + (curtime.tv_nsec-ttime.tv_nsec);
						if(elapsed < arrival_wait_time) my_sleep(arrival_wait_time - elapsed);
						//my_sleep(arrival_wait_time);
					}
					else {
						uint64_t arrival_wait_time = expwarmup(genwarmup);//gen_arrival_time(arrivalSleepTime);
						//printf("%llu \n", arrival_wait_time);
						clock_gettime(CLOCK_MONOTONIC,&curtime);
						uint64_t elapsed = (curtime.tv_sec-ttime.tv_sec )/1e-9 + (curtime.tv_nsec-ttime.tv_nsec);
						if(elapsed < arrival_wait_time) my_sleep(arrival_wait_time - elapsed);
						//my_sleep(arrival_wait_time);
					}
				//}
			}

			clock_gettime(CLOCK_MONOTONIC,&ttime);

			//runtime
			if(epochNumber == 0) {
				//if((ttime.tv_sec-epochStart.tv_sec)*1e9 + (ttime.tv_nsec-epochStart.tv_nsec) >= 1e9*3) {
                if(key_number == iters) {
					printf("T%llu warmup done \n", thread_id);
					epochNumber++;
                    key_number = 0;
					sleep(5);
					ret = pthread_barrier_wait(&barrier);
					clock_gettime(CLOCK_MONOTONIC,&realStart); 

					break;
				}
			}
			else {
				if((ttime.tv_sec-epochStart.tv_sec)*1e9 + (ttime.tv_nsec-epochStart.tv_nsec) >= 1e9*RUNTIME) {
					//printf("epoch %llu done \n", epochNumber);
					epochNumber++;
					break;
				}
			}
			

			#if SHARED_CQ
				struct ibv_wc wc[connPerThread*2*ctx->rx_depth];
			#else
				struct ibv_wc wc[2*ctx->rx_depth];
			#endif
			int ne, i;
			//printf("hello, thread id = %llu \n", thread_id);
			#if SHARED_CQ
				ne = ibv_poll_cq(sharedCQ[thread_id], connPerThread*2*ctx->rx_depth, wc);
				//printf("after poll cq \n");
			#else
				ne = ibv_poll_cq(ctx->cq, 2*ctx->rx_depth, wc);
			#endif
			//if(servername) printf("polling, ne = %llu \n", ne);
			if (ne < 0) {
				fprintf(stderr, "poll CQ failed %d\n", ne);
				return 0;
			}

			if (ne < 1) continue;

			for (i = 0; i < ne; ++i) {
				if (wc[i].status != IBV_WC_SUCCESS) {
					fprintf(stderr, "T%d - scnt = %llu, rcnt = %llu, Failed status %s (%d) for wr_id %d\n", thread_id, scnt, rcnt,
						ibv_wc_status_str(wc[i].status),
						wc[i].status, (int) wc[i].wr_id);
					return 0;
				}
				uint64_t wrID = (uint64_t) wc[i].wr_id;
				//printf("wrID = %llu \n", wrID);

				uint64_t connID = wc[i].qp_num & (MAX_QUEUES-1);

				#if SHARED_CQ
					//printf("connID = %llu \n", connID);
					ctx = ctxs[connID];
				#endif
				
				if(wrID == ctx->rx_depth) {
					ctx->scnt++;
					scnt++;
					//printf("send complete ... scnt = %llu \n", ctx->scnt);
				}
				else if (wrID >= 0 && wrID < ctx->rx_depth) {

					//printf("recv complete ... rcnt = %llu \n", ctx->rcnt);
					if(((uint8_t)ctx->buf_recv[wrID][0] >> 4) & 0x01 == 1) {
						countPriority[thread_id][connID]++;
						//printf("connID = %llu , qpn = %llu \n", connID, wc[i].qp_num);
						//printf("found valid pkt to measure diff \n");
						//handling userspace sequence number
						//handling userspace sequence number

						#if MEASURE_LATENCIES_ON_CLIENT						
						if(rcnt > 10000) {

							egressTS =  ((uint8_t)ctx->buf_recv[wrID][5] << 24) + ((uint8_t)ctx->buf_recv[wrID][6] << 16) + ((uint8_t)ctx->buf_recv[wrID][7] << 8) + ((uint8_t)ctx->buf_recv[wrID][8]);
							ingressTS = ((uint8_t)ctx->buf_recv[wrID][1] << 24) + ((uint8_t)ctx->buf_recv[wrID][2] << 16) + ((uint8_t)ctx->buf_recv[wrID][3] << 8) + ((uint8_t)ctx->buf_recv[wrID][4]);

							//if((uint8_t)ctx->buf_recv[wrID][9] >= numThreads) printf("coreid = %llu \n", (uint8_t)ctx->buf_recv[wrID][9]);
							
							//assert((uint8_t)ctx->buf_recv[wrID][9] < numThreads);

							whichcore[connID][receives[connID]] = (uint8_t)(((uint8_t)ctx->buf_recv[wrID][9] & 0xF0) >> 4);// (uint8_t)ctx->buf_recv[wrID][9];
							//whichcore[connID][receives[connID]] = (uint8_t)ctx->buf_recv[wrID][9] & 0x0F;

							if(egressTS > ingressTS) latency = egressTS - ingressTS;
							else latency = /*4294967295*/ uint64_t(1099511627775) + (egressTS - ingressTS);

							//printf("ingressTS = %llu, egressTS = %llu, latency = %llu \n", ingressTS, egressTS, latency);


							//printf("priority = %llu \n", priorityID);
							latencyArr[connID][receives[connID]] = latency;
							//printf("wrote latency \n");
							clock_gettime(CLOCK_MONOTONIC,&curtime);

							uint64_t elapsed = (curtime.tv_sec-realStart.tv_sec )/1e-9 + (curtime.tv_nsec-realStart.tv_nsec);
							latencyArrTimestamp[connID][receives[connID]] = elapsed;


							//printf("wrote latency timestamp \n");
							//if(receives[priorityID] < numExpectedRequests - 1) 

							#if debug						
							if(conn->rcnt % INTERVAL == 0 && conn->rcnt > 0) {
								double curr_clock = now();
								printf("T%d - %f rps, rcnt = %d, scnt = %d \n",thread_num,INTERVAL/(curr_clock-prev_clock),conn->rcnt,conn->scnt);
								prev_clock = curr_clock;
							}
							#endif

							//assert((uint8_t)connID == (uint8_t)thread_id);

							receives[connID]++;
							//assert((uint8_t)ctx->buf_recv[wrID][9] == 0 || (uint8_t)ctx->buf_recv[wrID][9] == 1 || (uint8_t)ctx->buf_recv[wrID][9] == 2 || (uint8_t)ctx->buf_recv[wrID][9] == 3);
						}
						#endif

						rcnt++;
						//assert(rcnt <= numExpectedRequestsPerThread);
						//assert(ctx->rcnt <= numExpectedRequestsPerConnection);
					}

					ctx->rcnt++;

					assert(pp_post_recv(ctx, wrID) == 0);
					if (ctx->routs < ctx->rx_depth) {
						fprintf(stderr,
							"Couldn't post receive (%d)\n", ctx->routs);
						return 0;
					}

					/*
					uint64_t success = pp_post_send(ctx);
					if (success == EINVAL) printf("Invalid value provided in wr \n");
					else if (success == ENOMEM)	printf("Send Queue is full or not enough resources to complete this operation \n");
					else if (success == EFAULT) printf("Invalid value provided in qp \n");
					else if (success != 0) {
						printf("success = %d, \n",success);
						fprintf(stderr, "Couldn't post send 3 \n");
						//return 1;
					}
					else {
						//printf("send posted ... sentCount = %llu, scnt = %llu \n", sentCount, conn->scnt);
					}
					*/
				}
				else {
					fprintf(stderr, "Completion for unknown wr_id %d\n",(int) wc[i].wr_id);
					return 0;
				}
			}
		}
	}

	ctx = ctxs[offset];

	float nsec = (ttime.tv_sec - realStart.tv_sec) * 1e9 + (ttime.tv_nsec - realStart.tv_nsec);
	//long long bytes = (long long) size * iters * 2;
		
	all_rcnts[thread_id] = rcnt;
	all_scnts[thread_id] = scnt;

	rps[thread_id] = rcnt/(nsec/1000000000.);
	printf("%d iters in %.5f seconds, rps = %f \n", rcnt, nsec/1000000000., rps[thread_id]);
}

int main(int argc, char *argv[])
{
	while (1) {
		int c;

		static struct option long_options[] = {
			{ .name = "long_query_percent",     .has_arg = 1, .flag = NULL, .val = 'p' },
			{ .name = "ib-dev",   .has_arg = 1, .flag = NULL, .val = 'd' },
			{ .name = "ib-port",  .has_arg = 1, .flag = NULL, .val = 'i' },
			{ .name = "size",     .has_arg = 1, .flag = NULL, .val = 's' },
			{ .name = "distribution_mode",      .has_arg = 1, .flag = NULL, .val = 'm' },
			{ .name = "priority_distribution_mode",      .has_arg = 1, .flag = NULL, .val = 'z' },
			{ .name = "rx-depth", .has_arg = 1, .flag = NULL, .val = 'r' },
			{ .name = "iters",    .has_arg = 1, .flag = NULL, .val = 'n' },
			{ .name = "load",     .has_arg = 1, .flag = NULL, .val = 'l' },
			{ .name = "events",   .has_arg = 0, .flag = NULL, .val = 'e' },
			{ .name = "gid-idx",  .has_arg = 1, .flag = NULL, .val = 'g' },
			{ .name = "chk",      .has_arg = 0, .flag = NULL, .val = 'c' },
			{ .name = "bimodal_ratio",        .has_arg = 0, .flag = NULL, .val = 'x' },
			{ .name = "num-threads", .has_arg = 1, .flag = NULL, .val = 't' },
			{}
		};

		c = getopt_long(argc, argv, "p:d:i:s:m:z:r:n:l:eg:c:x:t:f:",
				long_options, NULL);
		if (c == -1)
			break;

		switch (c) {
		case 'p':
			long_query_percent = strtoul(optarg, NULL, 0);
			/*
			port = strtoul(optarg, NULL, 0);
			if (port > 65535) {
				usage(argv[0]);
				return 1;
			}
			*/
			break;

		case 'd':
			ib_devname = strdupa(optarg);
			break;

		case 'i':
			ib_port = strtol(optarg, NULL, 0);
			if (ib_port < 1) {
				usage(argv[0]);
				return 1;
			}
			break;

		case 's':
			size = strtoul(optarg, NULL, 0);
			break;

		case 'm':
			distribution_mode = strtoul(optarg, NULL, 0);
			break;

		case 'z':
			priority_distribution_mode = strtoul(optarg, NULL, 0);
			break;

		case 'r':
			rx_depth = strtoul(optarg, NULL, 0);
			break;

		case 'n':
			numConnections = strtoul(optarg, NULL, 0);
			break;

		case 'l':
			goalLoad = strtol(optarg, NULL, 0);
			break;

		case 'g':
			gidx = strtol(optarg, NULL, 0);
			break;

		case 'x':
			bimodal_ratio = strtoul(optarg, NULL, 0);
			break;

		case 't':
			numThreads = strtoul(optarg, NULL, 0);
			break;
      
	  	case 'f':
        	output_dir = optarg;
        	break;
		
		default:
			usage(argv[0]);
			return 1;
		}
	}

	#if ENABLE_KERNEL
	uint64_t sizeofArray = 3008;
	uint64_t numIterations = 10000;
	uint64_t numPartitions = 32;
	//600*1024*8 = 4.6 MB
	//2400*1024*8 = 18.75 MB
	//3008*1024*8 = 18.75 MB

	std::random_device rd1{};
	std::mt19937 gen1{rd1()};
	std::uniform_int_distribution<uint8_t>* rand1 = new std::uniform_int_distribution<uint8_t>(0, 255);

	connArray = (uint64_t**)malloc(numConnections*sizeof(uint64_t*));

	for(uint64_t cc = 0; cc < numConnections; cc++){
		connArray[cc] = (uint64_t*)malloc(sizeofArray*sizeof(uint64_t));
		for(uint64_t ccc = 0; ccc < sizeofArray; ccc++){
			connArray[cc][ccc] = (*rand1)(gen1);
		}
	}
	#endif

	if (optind == argc - 1)
		servername = strdupa(argv[optind]);
	else if (optind < argc) {
		usage(argv[0]);
		return 1;
	}
	
	ret = pthread_barrier_init(&barrier, &attr, numThreads);
	assert(ret == 0);

	//numExpectedRequestsPerThread = goalLoad*RUNTIME*numEpochs/numThreads;
	if(numConnections > 1) numExpectedRequestsPerConnection = goalLoad*RUNTIME*numEpochs/(numConnections/2);// /numConnections
	else numExpectedRequestsPerConnection = goalLoad*RUNTIME*numEpochs/(numConnections);

	page_size = sysconf(_SC_PAGESIZE);

	dev_list = ibv_get_device_list(NULL);
	if (!dev_list) {
		perror("Failed to get IB devices list");
		return 1;
	}

	if (!ib_devname) {
		ib_dev = *dev_list;
		if (!ib_dev) {
			fprintf(stderr, "No IB devices found\n");
			return 1;
		}
	} else {
		int i;
		for (i = 0; dev_list[i]; ++i)
			if (!strcmp(ibv_get_device_name(dev_list[i]), ib_devname))
				break;
		ib_dev = dev_list[i];
		if (!ib_dev) {
			fprintf(stderr, "IB device %s not found\n", ib_devname);
			return 1;
		}
	}

	window_per_thread = rx_depth/numThreads;
	
	double totalRPS = 0;
	rps = (double*)malloc((numConnections)*sizeof(double));
	memset(rps, 0, numConnections*sizeof(double));

	connPerThread = numConnections/numThreads;
	printf("connections per thread = %llu \n", connPerThread);
	#if SHARED_CQ
		sharedCQ = (struct ibv_cq **)malloc(numThreads*sizeof(struct ibv_cq *));
	#endif
	
	ctx = (struct pingpong_context**)malloc(numConnections*sizeof(struct pingpong_context*));

	for(uint64_t y = 0; y < numConnections; y++) {
		my_sleep(uint64_t(5E6));
		//memset(ctx[y], 0, sizeof(struct pingpong_context*));
		//printf("y = %llu \n", y);
		ctx[y]  = pp_init_ctx(ib_dev, size, rx_depth, ib_port);
		if (!ctx[y])
			return 1;
		//printf("hello 22 \n");

		ctx[y]->id = y;
		ctx[y]->routs = 0;
		ctx[y]->souts = 0;
		ctx[y]->scnt = 0;
		ctx[y]->rcnt = 0;

		for(uint64_t z = 0; z < ctx[y]->rx_depth; z++) {
			//printf("hello 222 \n");
			assert(pp_post_recv(ctx[y], z) == 0);
			//printf("hello 3 \n");
			ctx[y]->routs++;
		}
		//printf("hello 2222 \n");

		if (ctx[y]->routs < ctx[y]->rx_depth) {
			fprintf(stderr, "Couldn't post receive (%d)\n", ctx[y]->routs);
			return 1;
		}
		
		//printf("hello 0 \n");

		if (pp_get_port_info(ctx[y]->context, ib_port, &ctx[y]->portinfo)) {
			fprintf(stderr, "Couldn't get port info\n");
			return 1;
		}
		//printf("hello 1 \n");

		ctx[y]->my_dest.lid = ctx[y]->portinfo.lid;
		if (ctx[y]->portinfo.link_layer != IBV_LINK_LAYER_ETHERNET &&
								!ctx[y]->my_dest.lid) {
			fprintf(stderr, "Couldn't get local LID\n");
			return 1;
		}

		if (gidx >= 0) {
			if (ibv_query_gid(ctx[y]->context, ib_port, gidx, &ctx[y]->my_dest.gid)) {
				fprintf(stderr, "can't read sgid of index %d\n", gidx);
				return 1;
			}
		} else
			memset(&ctx[y]->my_dest.gid, 0, sizeof ctx[y]->my_dest.gid);

		ctx[y]->my_dest.qpn = ctx[y]->qp->qp_num;
		ctx[y]->my_dest.psn = 0;//lrand48() & 0xffffff;
		inet_ntop(AF_INET6, &ctx[y]->my_dest.gid, gid, sizeof gid);
		ctx[y]->my_dest.addr = (uintptr_t)ctx[y]->buf_write;
		//printf("hello y = %llu \n", y);
		ctx[y]->my_dest.rkey = ctx[y]->mr_write->rkey;
		
		//printf("hello 111 \n");

		//printf("  local address:  LID 0x%04x, QPN 0x%06x, PSN 0x%06x, GID %s, ADDR %p, RKEY 0x%08x \n",
		//	ctx[y]->my_dest.lid, ctx[y]->my_dest.qpn, ctx[y]->my_dest.psn, gid, ctx[y]->my_dest.addr, ctx[y]->my_dest.rkey);


		if (servername)
			ctx[y]->rem_dest = pp_client_exch_dest(servername, port, &ctx[y]->my_dest);
		else
			ctx[y]->rem_dest = pp_server_exch_dest(ctx[y], ib_port, mtu, port, sl, &ctx[y]->my_dest, gidx);

		if (!ctx[y]->rem_dest)
			return 1;

		inet_ntop(AF_INET6, &ctx[y]->rem_dest->gid, gid, sizeof gid);
		//printf("  remote address: LID 0x%04x, QPN 0x%06x, PSN 0x%06x, GID %s, ADDR %p, RKEY 0x%08x\n",
		//	ctx[y]->rem_dest->lid, ctx[y]->rem_dest->qpn, ctx[y]->rem_dest->psn, gid, ctx[y]->rem_dest->addr, ctx[y]->rem_dest->rkey);

		if (servername)
			if (pp_connect_ctx(ctx[y], ib_port, ctx[y]->my_dest.psn, mtu, sl, ctx[y]->rem_dest,
						gidx))
				return 1;
		
		/*
		if (servername) {
			for(uint64_t z = 0; z < ctx[y]->rx_depth; z++) {
				uint64_t success = pp_post_send(ctx[y]);
				if (success == EINVAL) printf("Invalid value provided in wr \n");
				else if (success == ENOMEM)	printf("Send Queue is full or not enough resources to complete this operation \n");
				else if (success == EFAULT) printf("Invalid value provided in qp \n");
				else if (success != 0) {
					printf("success = %d, \n",success);
					fprintf(stderr, "Couldn't post send 3 \n");
					//return 1;
				}
				else {
					//printf("send posted ... z = %llu \n", z);
				}
				//double interArrivalWaitTime = ran_expo(1/1000);
				//printf("%f \n", interArrivalWaitTime);
			}
		}
		*/

		//if(servername) sleep(1);
		//ctx[y]->rcnt = ctx[y]->scnt = 0;

	}

	pthread_t pt[numConnections];
    int ret;

	//numThreads = numConnections;

    rps = (double*)malloc((numThreads)*sizeof(double));
	all_rcnts = (uint32_t*)malloc(numThreads*sizeof(uint32_t));
	all_scnts = (uint32_t*)malloc(numThreads*sizeof(uint32_t));
	countPriority = (uint64_t **)malloc(numThreads*sizeof(uint64_t *));

	for(int t = 0; t < numThreads; t++) {
		countPriority[t] = (uint64_t *)malloc(numConnections*sizeof(uint64_t));
		for(int g = 0; g < numConnections; g++) countPriority[t][g] = 0;
	}

	countSendPriority = (uint64_t **)malloc(numThreads*sizeof(uint64_t *));
	for(int t = 0; t < numThreads; t++) {
		countSendPriority[t] = (uint64_t *)malloc(numConnections*sizeof(uint64_t));
		for(int g = 0; g < numConnections; g++) countSendPriority[t][g] = 0;
	}

	expectedSeqNum = (uint8_t *)malloc(numConnections*sizeof(uint8_t));
	for(int c = 0; c < numConnections; c++) expectedSeqNum[c] = 0;


	#if MEASURE_LATENCIES_ON_CLIENT
	//latencies.resize(active_thread_num, vector<vector<uint64_t>>(numberOfPriorities, vector<uint64_t>(0,0)));

	recvDistribution = (uint64_t**)malloc(numThreads*sizeof(uint64_t*));
	for(int i = 0; i < numThreads; i++) {
		recvDistribution[i] = (uint64_t*)malloc(numConnections*sizeof(uint64_t));
		for(int j = 0; j < numConnections; j++) recvDistribution[i][j] = 0;
	}

	//allocating rcnt, scnt, souts
	rcnt = (uint64_t *)malloc(numThreads*sizeof(uint64_t));
	scnt = (uint64_t *)malloc(numThreads*sizeof(uint64_t));
	souts = (uint64_t *)malloc(numThreads*sizeof(uint64_t));
	largestCoreDepthDifference = (uint64_t *)malloc(numThreads*sizeof(uint64_t));

	for(uint64_t bb = 0; bb < numThreads; bb++) {
		rcnt[bb] = 0;
		scnt[bb] = 0;
		souts[bb] = 0;
		largestCoreDepthDifference[bb] = 0;
	}	

	//printf("allocating latencies, size of latency = %llu \n", sizeof(uint64_t));
	latencies = (uint64_t ***)malloc(numThreads*sizeof(uint64_t**));
	size_t total_mem_allocated = 0;
	for(uint64_t b = 0; b < numThreads; b++) {
		//std::cout << "Allocating latencies[" << b << "]: " << numConnections * sizeof(uint64_t) << std::endl;
		total_mem_allocated += numConnections * sizeof(uint64_t);
		latencies[b] = (uint64_t**)malloc(numConnections*sizeof(uint64_t*));
		for(uint64_t p = 0; p < numConnections; p++) {
			//std::cout << "Allocating latencies[" << b << "][" << p << "]: " << numConnections * sizeof(uint64_t) << std::endl;
			latencies[b][p] = (uint64_t*)malloc(numExpectedRequestsPerConnection*sizeof(uint64_t));
			total_mem_allocated += numExpectedRequestsPerConnection*sizeof(uint64_t);
			for(uint64_t v = 0; v < numExpectedRequestsPerConnection; v++) {
				latencies[b][p][v] = 0;
			}
		}
	}

	std::cout << "Interim total mem allocated (MiB): " << (total_mem_allocated / 1024 / 1024) << std::endl;

	//printf("allocating latencies timestamps, size of timestamp = %llu \n", sizeof(uint64_t));
	latenciesTimestamp = (uint64_t ***)malloc(numThreads*sizeof(uint64_t **));
	for(uint64_t b = 0; b < numThreads; b++) {
		latenciesTimestamp[b] = (uint64_t **)malloc(numConnections*sizeof(uint64_t *));
		total_mem_allocated += numConnections * sizeof(uint64_t *);
		for(uint64_t p = 0; p < numConnections; p++) {
			latenciesTimestamp[b][p] = (uint64_t *)malloc(numExpectedRequestsPerConnection*sizeof(uint64_t));
			total_mem_allocated += numExpectedRequestsPerConnection * sizeof(uint64_t);
			for(uint64_t v = 0; v < numExpectedRequestsPerConnection; v++) {
				latenciesTimestamp[b][p][v] = 0;
			}
		}
	}
	printf("done allocating latencies timestamps \n");

	whichcore = (uint8_t ***)malloc(numThreads*sizeof(uint8_t **));
	for(uint8_t b = 0; b < numThreads; b++) {
		whichcore[b] = (uint8_t **)malloc(numConnections*sizeof(uint8_t *));
		total_mem_allocated += numConnections * sizeof(uint8_t *);
		for(uint16_t p = 0; p < numConnections; p++) {
			whichcore[b][p] = (uint8_t *)malloc(numExpectedRequestsPerConnection*sizeof(uint8_t));
			total_mem_allocated += numExpectedRequestsPerConnection * sizeof(uint8_t);
			for(uint64_t v = 0; v < numExpectedRequestsPerConnection; v++) {
				whichcore[b][p][v] = 0;
			}
		}
	}

	printf("done allocating whichcore \n");


	std::random_device rd1;
	std::mt19937 gen1(rd1());

	if(bimodal_ratio == 50) {
		discreteDist[1] = 100-long_query_percent; //GET
		discreteDist[0] = long_query_percent;     //PUT
	}
	else if(bimodal_ratio == 33) {
		discreteDist[1] = 34;
		discreteDist[5] = 33;
		discreteDist[3] = 33;
	}    
	else if(bimodal_ratio == 13) {
		discreteDist[1] = 100-long_query_percent;
		discreteDist[6] = long_query_percent;
	}
	else if(bimodal_ratio == 10) {
		discreteDist[1] = 100-long_query_percent;
		discreteDist[5] = long_query_percent;
	}   
	else if(bimodal_ratio == 6) {
		discreteDist[1] = 100-long_query_percent;
		discreteDist[4] = long_query_percent;
	}
	else if(bimodal_ratio == 3) {
		discreteDist[1] = 100-long_query_percent;
		discreteDist[3] = long_query_percent;
	}
	else if(bimodal_ratio == 1) {
		discreteDist[1] = 100-long_query_percent;
		discreteDist[2] = long_query_percent;
	}
	else {
		printf("INVALID BIMODAL DISPARITY RATIO! Valid options include: 2,5,10,15,20 \n");
	}

	random_keys = (uint32_t**)malloc(numConnections*sizeof(uint32_t*));
	for(uint16_t p = 0; p < numConnections; p++) {
		random_keys[p] = (uint32_t *)malloc(connKeyArraySize*sizeof(uint32_t));
	}

	printf("done with random_keys \n");

	op_type = (uint8_t**)malloc(numConnections*sizeof(uint8_t*));
	for(uint16_t p = 0; p < numConnections; p++) {
		op_type[p] = (uint8_t *)malloc(connKeyArraySize*sizeof(uint8_t));
	}

	printf("done with op_type \n");

	std::discrete_distribution<> d1({discreteDist[0],discreteDist[1],discreteDist[2],discreteDist[3],discreteDist[4],discreteDist[5],discreteDist[6]});        
	//printf("hello \n");

	for (uint16_t ii = 0; ii < numConnections; ii++) {
		//uniform random
		std::uniform_int_distribution<uint32_t>* urkeys = new std::uniform_int_distribution<uint32_t>(keysPerConnection*ii, (keysPerConnection*(ii+1))-1);

		//100% CAP RW-LLC
		//std::uniform_int_distribution<uint32_t>* urkeys = new std::uniform_int_distribution<uint32_t>(keysPerConnection*ii, (keysPerConnection*ii)+845);
		//10% CAP RW-LLC
		//std::uniform_int_distribution<uint32_t>* urkeys = new std::uniform_int_distribution<uint32_t>(keysPerConnection*ii, (keysPerConnection*ii)+8409);

		for(uint64_t p = 0; p < connKeyArraySize; p++) {
			random_keys[ii][p] = (*urkeys)(gen1);
			op_type[ii][p] = d1(gen1);
		}
	}

	std::cout << "Final total mem allocated: " << (total_mem_allocated / 1024 / 1024) << std::endl;

	#endif


	std::random_device rd{};
	std::mt19937 gen{rd()};

	uint16_t numQueuesGettingMoreLoad;
	std::uniform_int_distribution<uint64_t>* rand = new std::uniform_int_distribution<uint64_t>(0, numConnections-1);


	for(uint16_t e = 0; e < numEpochs; e++) {

		if(priority_distribution_mode == FIFTYPRAND) {
			numQueuesGettingMoreLoad = numConnections/2;
			// Generate unique numbers
			while (uniqueNumbers50P[e].size() < numQueuesGettingMoreLoad) {
				int num = (*rand)(gen);
				uniqueNumbers50P[e].insert(num);  // Insert only if it's unique
			}
		}
		else if (priority_distribution_mode == TWENTYPRAND) {
			numQueuesGettingMoreLoad = numConnections/5;
			// Generate unique numbers
			while (uniqueNumbers20P[e].size() < numQueuesGettingMoreLoad) {
				int num = (*rand)(gen);
				uniqueNumbers20P[e].insert(num);  // Insert only if it's unique
			}
			/*
			for (auto it = uniqueNumbers20P[e].begin(); it != uniqueNumbers20P[e].end(); ++it) {
				std::cout << *it << " ";
			}
			std::cout << std::endl;
			*/
		}
		else if(priority_distribution_mode == TENPRAND) {
			numQueuesGettingMoreLoad = numConnections/10;
			// Generate unique numbers
			while (uniqueNumbers10P[e].size() < numQueuesGettingMoreLoad) {
				int num = (*rand)(gen);
				uniqueNumbers10P[e].insert(num);  // Insert only if it's unique
			}
		}
		else if(priority_distribution_mode == HUNDREDPRAND) {
			numQueuesGettingMoreLoad = numConnections;
			// Generate unique numbers
			while (uniqueNumbers100P[e].size() < numQueuesGettingMoreLoad) {
				int num = (*rand)(gen);
				uniqueNumbers100P[e].insert(num);  // Insert only if it's unique
			}
		}
		else if(priority_distribution_mode == MIXEDRAND) {

			numQueuesGettingMoreLoad = numConnections/2;
			// Generate unique numbers
			while (uniqueNumbers50P[e].size() < numQueuesGettingMoreLoad) {
				int num = (*rand)(gen);
				uniqueNumbers50P[e].insert(num);  // Insert only if it's unique
			}

			numQueuesGettingMoreLoad = numConnections/5;
			// Generate unique numbers
			while (uniqueNumbers20P[e].size() < numQueuesGettingMoreLoad) {
				int num = (*rand)(gen);
				uniqueNumbers20P[e].insert(num);  // Insert only if it's unique
			}

			numQueuesGettingMoreLoad = numConnections/10;
			// Generate unique numbers
			while (uniqueNumbers10P[e].size() < numQueuesGettingMoreLoad) {
				int num = (*rand)(gen);
				uniqueNumbers10P[e].insert(num);  // Insert only if it's unique
			}
		
			numQueuesGettingMoreLoad = numConnections;
			// Generate unique numbers
			while (uniqueNumbers100P[e].size() < numQueuesGettingMoreLoad) {
				int num = (*rand)(gen);
				uniqueNumbers100P[e].insert(num);  // Insert only if it's unique
			}
		}
	}


	do_uc(ib_devname, servername, port, ib_port, gidx, numConnections, numThreads);

	//if(servername == NULL) numThreads = 1;
    for(int x = 0; x < numThreads; x++) {
		struct thread_data *tdata = (struct thread_data *)malloc(sizeof(struct thread_data));

		tdata->ctxs = ctx;
		tdata->thread_id = x;

		#if MEASURE_LATENCIES_ON_CLIENT
		tdata->latencyArr = latencies[x];
		tdata->latencyArrTimestamp = latenciesTimestamp[x];
		tdata->receives = recvDistribution[x];
		tdata->whichcore = whichcore[x];
		tdata->rcnt = rcnt[x];
		tdata->scnt = scnt[x];		
		tdata->souts = souts[x];
		#endif  

		ret = pthread_create(&pt[x], NULL, threadfunc, tdata); 
		assert(ret == 0);
	}

    for(int x = 0; x < numThreads; x++) {
		//printf("x = %llu \n", x);
        int ret = pthread_join(pt[x], NULL);
        assert(!ret);
    }

	uint64_t largestCoreDepthGap = 0;
	for(int g = 0; g < numThreads; g++) {
		if(largestCoreDepthDifference[g] > largestCoreDepthGap) largestCoreDepthGap = largestCoreDepthDifference[g];
	}
	printf("largestCoreDepthDifference = %llu \n", largestCoreDepthGap);


	uint32_t total_rcnt = 0;
	uint32_t total_scnt = 0;

	for(int i = 0; i < numThreads; i++){
		//printf("allrcnt[%d] = %llu \n", i, all_rcnts[i]);
		total_rcnt += all_rcnts[i];
		total_scnt += all_scnts[i];
	}


	uint64_t *totalPerThread = (uint64_t *)malloc(numThreads*sizeof(uint64_t));
	for(int t = 0; t < numThreads; t++) totalPerThread[t] = 0;

	printf("total received per connection ... \n");
	for(int g = 0; g < numConnections; g++) {
		printf("P%llu   ",g);

		for(int t = 0; t < numThreads; t++){
			totalPerThread[t] += countPriority[t][g];
			printf("%llu    ",countPriority[t][g]);
		}
		printf("\n");
	}		
	printf("\n");
	printf("total received per thread ... \n");
	for(int t = 0; t < numThreads; t++){
		printf("%llu   ",totalPerThread[t]);
	}

	printf("\n");
	printf("\n");
	
	uint64_t *totalSendPerThread = (uint64_t *)malloc(numThreads*sizeof(uint64_t));
	for(int t = 0; t < numThreads; t++) totalSendPerThread[t] = 0;

	printf("total sent per connection ... \n");
	for(int g = 0; g < numConnections; g++) {
		printf("P%llu   ",g);
		for(int t = 0; t < numThreads; t++){
			totalSendPerThread[t] += countSendPriority[t][g];
			printf("%llu    ",countSendPriority[t][g]);
		}
		printf("\n");
	}		
	printf("\n");
	printf("total sent per thread ... \n");
	for(int t = 0; t < numThreads; t++){
		printf("%llu   ",totalSendPerThread[t]);
	}



	printf("\n");
	printf("total rcnt = %d, total scnt = %d \n",(int)total_rcnt,(int)total_scnt);


	for(int i = 0; i < numThreads; i++){
		printf("rps = %f \n",rps[i]);
		totalRPS += rps[i];
	}
	printf("%f \n",totalRPS);

	
	#if MEASURE_LATENCIES_ON_CLIENT
	char* output_name_all;
	char* output_name_all_timestamps;
	char* output_name_conn;
	char* output_name_core;
	//char* output_name_qd;
	//char* output_name_diffqd;

	asprintf(&output_name_all, "%s/all_%d_%d_conn.result", output_dir, (int)totalRPS, numConnections);
	asprintf(&output_name_all_timestamps, "%s/all_%d_%d_conn.timestamps", output_dir, (int)totalRPS, numConnections);
	asprintf(&output_name_conn, "%s/all_%d_%d_conn.conn", output_dir, (int)totalRPS, numConnections);
	asprintf(&output_name_core, "%s/all_%d_%d_conn.core", output_dir, (int)totalRPS, numConnections);
	//asprintf(&output_name_qd, "%s/all_%d_%d_conn.qd", output_dir, (int)totalRPS, numConnections);
	//asprintf(&output_name_diffqd, "%s/all_%d_%d_conn.diffqd", output_dir, (int)totalRPS, numConnections);

	FILE *f_all = fopen(output_name_all, "wb");
	FILE *f_all_timestamps = fopen(output_name_all_timestamps, "wb");
	FILE *f_conn = fopen(output_name_conn, "wb");
	FILE *f_core = fopen(output_name_core, "wb");
	//FILE *f_qd = fopen(output_name_qd, "wb");
	//FILE *f_diffqd = fopen(output_name_diffqd, "wb");


	uint64_t countedLatencyZero = 0;
	printf("dumping latencies ... \n");
	for(uint i = 0; i < numConnections; i++){
		for(uint8_t j = 0; j < numThreads; j++) {
			//while(latencies[j][i][index] > 0) {
			for(uint64_t index = 0; index < recvDistribution[j][i]; index++) {
				if(latencies[j][i][index] == 0 || latenciesTimestamp[j][i][index] == 0 || latenciesTimestamp[j][i][index] > 2*numEpochs*RUNTIME*1e9) {
					countedLatencyZero++;
					continue;
				}
				//fprintf(f, "%llu \n", latencies[j][i][index]);
				fprintf(f_all, "%llu \n", latencies[j][i][index]);
				fprintf(f_all_timestamps, "%llu \n", latenciesTimestamp[j][i][index]);
				fprintf(f_conn, "%llu \n", i);

				if(whichcore[j][i][index] >= numThreads) printf("coreid = %llu \n", whichcore[j][i][index]);
				assert(whichcore[j][i][index] < numThreads);
				fprintf(f_core, "%llu \n", whichcore[j][i][index]);
			}
		}
		//fclose(f);
	}
	printf("count of zero latency = %llu \n",countedLatencyZero);
	fclose(f_all);
	fclose(f_all_timestamps);
	fclose(f_conn);
	fclose(f_core);
	//fclose(f_qd);
	//fclose(f_diffqd);
	#endif
	
	
	//printf("closing connections \n");
	/*
	for(uint64_t z = 0; z < numConnections; z++) {
		if (pp_close_ctx(ctx[z]))
			return 1;
		//printf("z = %d \n", z);
	}
	*/
	//printf("closed connections \n");

	//ibv_free_device_list(dev_list);
	//printf("free device list \n");

	return 0;
}