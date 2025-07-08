/* Masstree
 * Eddie Kohler, Yandong Mao, Robert Morris
 * Copyright (c) 2012-2014 President and Fellows of Harvard College
 * Copyright (c) 2012-2014 Massachusetts Institute of Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, subject to the conditions
 * listed in the Masstree LICENSE file. These conditions include: you must
 * preserve this copyright notice, and you cannot mention the copyright
 * holders in advertising related to the Software without their permission.
 * The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
 * notice is a summary of the Masstree LICENSE file; the license in that file
 * is legally binding.
 */
// -*- mode: c++ -*-
// mtd: key/value server
//

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <limits.h>
#if HAVE_SYS_EPOLL_H
#include <sys/epoll.h>
#endif
#if __linux__
#include <asm-generic/mman.h>
#endif
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <signal.h>
#include <errno.h>
#ifdef __linux__
#include <malloc.h>
#endif
#include "nodeversion.hh"
#include "kvstats.hh"
#include "json.hh"
#include "kvtest.hh"
#include "kvrandom.hh"
#include "clp.h"
#include "log.hh"
#include "checkpoint.hh"
#include "file.hh"
#include "kvproto.hh"
#include "query_masstree.hh"
#include "masstree_tcursor.hh"
#include "masstree_insert.hh"
#include "masstree_remove.hh"
#include "masstree_scan.hh"
#include "msgpack.hh"
#include <algorithm>
#include <deque>
using lcdf::StringAccum;

enum { CKState_Quit, CKState_Uninit, CKState_Ready, CKState_Go };

volatile bool timeout[2] = {false, false};
double duration[2] = {10, 0};

Masstree::default_table *tree;

// all default to the number of cores
static int udpthreads = 0;
static int tcpthreads = 0;
static int nckthreads = 0;
static int testthreads = 0;
static int nlogger = 0;
static std::vector<int> cores;

static bool logging = true;
static bool pinthreads = false;
static bool recovery_only = false;
relaxed_atomic<mrcu_epoch_type> globalepoch(1);     // global epoch, updated by main thread regularly
relaxed_atomic<mrcu_epoch_type> active_epoch(1);
//static int port = 2117;
static uint64_t test_limit = ~uint64_t(0);
static int doprint = 0;
int kvtest_first_seed = 31949;

static volatile sig_atomic_t go_quit = 0;
static int quit_pipe[2];

static std::vector<const char*> logdirs;
static std::vector<const char*> ckpdirs;

static logset* logs;
volatile bool recovering = false; // so don't add log entries, and free old value immediately

static double checkpoint_interval = 1000000;
static kvepoch_t ckp_gen = 0; // recover from checkpoint
static ckstate *cks = NULL; // checkpoint status of all checkpointing threads
static pthread_cond_t rec_cond;
pthread_mutex_t rec_mu;
static int rec_nactive;
static int rec_state = REC_NONE;

kvtimestamp_t initial_timestamp;

static pthread_cond_t checkpoint_cond;
static pthread_mutex_t checkpoint_mu;

static void prepare_thread(threadinfo *ti);
static int* tcp_thread_pipes;
static void* tcp_threadfunc(void* ti);
static void* udp_threadfunc(void* ti);

static void log_init();
static void recover(threadinfo*);
static kvepoch_t read_checkpoint(threadinfo*, const char *path);

static void* conc_checkpointer(void* ti);
static void recovercheckpoint(threadinfo* ti);

static void *canceling(void *);
static void catchint(int);
static void epochinc(int);



#include <linux/types.h>  //for __be32 type
#include <random>
#include "rdma_uc.cc"
#include <x86intrin.h> // For __rdtsc()
#include <infiniband/verbs.h>
#define SERVICE_TIME_SIZE 0x8000

#define MAX_QUEUES 1024
#define SHARED_CQ 0
#define RC 1
#define ENABLE_SERV_TIME 1
#define FPGA_NOTIFICATION 1
#define debugFPGA 0
#define bitVector 1

#define SHARE_SHAREDCQs 0

#define NOTIFICATION_QUEUE 0

#define CONN_AFFINITY_OVERHEAD 0

#define ENABLE_KERNEL 0

#define RUNTIME 45 //115

struct ibv_device      **dev_list;
struct ibv_device	*ib_dev;
char                    *ib_devname = NULL;
char                    *servername = NULL;
unsigned int             port = 18512;
int                      ib_port = 1;
unsigned int             size = 1024;
enum ibv_mtu		 	 mtu = IBV_MTU_1024;
uint64_t             rx_depth = 8192;
uint64_t             iters = 1000;
int                      num_cq_events = 0;
int                      sl = 0;
int			 gidx = -1;
char			 gid[33];

uint64_t numConnections = 48;

uint32_t * all_rcnts;
uint32_t * all_scnts;
uint64_t **countPriority;
double *rps;
uint64_t goalLoad = 0;
uint64_t numThreads = 0;

uint64_t connPerThread = 0;

uint8_t *expectedSeqNum;


/* running local tests */
void test_timeout(int) {
    size_t n;
    for (n = 0; n < arraysize(timeout) && timeout[n]; ++n)
        /* do nothing */;
    if (n < arraysize(timeout)) {
        timeout[n] = true;
        if (n + 1 < arraysize(timeout) && duration[n + 1])
            xalarm(duration[n + 1]);
    }
}

struct kvtest_client {
    kvtest_client()
        : checks_(0), kvo_() {
    }
    kvtest_client(const char *testname)
        : testname_(testname), checks_(0), kvo_() {
    }

    int id() const {
        return ti_->index();
    }
    int nthreads() const {
        return testthreads;
    }
    void set_thread(threadinfo *ti) {
        ti_ = ti;
    }
    void register_timeouts(int n) {
        always_assert(n <= (int) arraysize(::timeout));
        for (int i = 1; i < n; ++i)
            if (duration[i] == 0)
                duration[i] = 0;//duration[i - 1];
    }
    bool timeout(int which) const {
        return ::timeout[which];
    }
    uint64_t limit() const {
        return test_limit;
    }
    Json param(const String&) const {
        return Json();
    }
    double now() const {
        return ::now();
    }

    void get(long ikey, Str *value);
    void get(const Str &key);
    void get(long ikey) {
        quick_istr key(ikey);
        get(key.string());
    }
    void get_check(const Str &key, const Str &expected);
    void get_check(const char *key, const char *expected) {
        get_check(Str(key, strlen(key)), Str(expected, strlen(expected)));
    }
    void get_check(long ikey, long iexpected) {
        quick_istr key(ikey), expected(iexpected);
        get_check(key.string(), expected.string());
    }
    void get_check_key8(long ikey, long iexpected) {
        quick_istr key(ikey, 8), expected(iexpected);
        get_check(key.string(), expected.string());
    }
    void get_check_key10(long ikey, long iexpected) {
        quick_istr key(ikey, 10), expected(iexpected);
        get_check(key.string(), expected.string());
    }
    void get_col_check(const Str &key, int col, const Str &expected);
    void get_col_check_key10(long ikey, int col, long iexpected) {
        quick_istr key(ikey, 10), expected(iexpected);
        get_col_check(key.string(), col, expected.string());
    }
    bool get_sync(long ikey);

    void put(const Str &key, const Str &value);
    void put(const char *key, const char *val) {
        put(Str(key, strlen(key)), Str(val, strlen(val)));
    }
    void put(long ikey, long ivalue) {
        quick_istr key(ikey), value(ivalue);
        put(key.string(), value.string());
    }
    void put_key8(long ikey, long ivalue) {
        quick_istr key(ikey, 8), value(ivalue);
        put(key.string(), value.string());
    }
    void put_key10(long ikey, long ivalue) {
        quick_istr key(ikey, 10), value(ivalue);
        put(key.string(), value.string());
    }
    void put_col(const Str &key, int col, const Str &value);
    void put_col_key10(long ikey, int col, long ivalue) {
        quick_istr key(ikey, 10), value(ivalue);
        put_col(key.string(), col, value.string());
    }

    bool remove_sync(long ikey);

    void puts_done() {
    }
    void wait_all() {
    }
    void rcu_quiesce() {
    }
    String make_message(StringAccum &sa) const;
    void notice(const char *fmt, ...);
    void fail(const char *fmt, ...);
    const Json& report(const Json& x) {
        return report_.merge(x);
    }
    void finish() {
        fprintf(stderr, "%d: %s\n", ti_->index(), report_.unparse().c_str());
    }
    threadinfo *ti_;
    query<row_type> q_[10];
    const char *testname_;
    kvrandom_lcg_nr rand;
    int checks_;
    Json report_;
    struct kvout *kvo_;
    static volatile int failing;
};

volatile int kvtest_client::failing;

void kvtest_client::get(long ikey, Str *value)
{
    quick_istr key(ikey);
    if (!q_[0].run_get1(tree->table(), key.string(), 0, *value, *ti_))
        *value = Str();
}

void kvtest_client::get(const Str &key)
{
    Str val;
    (void) q_[0].run_get1(tree->table(), key, 0, val, *ti_);
}

void kvtest_client::get_check(const Str &key, const Str &expected)
{
    Str val;
    if (!q_[0].run_get1(tree->table(), key, 0, val, *ti_)) {
        fail("get(%.*s) failed (expected %.*s)\n", key.len, key.s, expected.len, expected.s);
        return;
    }
    if (val.len != expected.len || memcmp(val.s, expected.s, val.len) != 0)
        fail("get(%.*s) returned unexpected value %.*s (expected %.*s)\n", key.len, key.s,
             std::min(val.len, 40), val.s, std::min(expected.len, 40), expected.s);
    else
        ++checks_;
}

void kvtest_client::get_col_check(const Str &key, int col, const Str &expected)
{
    Str val;
    if (!q_[0].run_get1(tree->table(), key, col, val, *ti_)) {
        fail("get.%d(%.*s) failed (expected %.*s)\n", col, key.len, key.s,
             expected.len, expected.s);
        return;
    }
    if (val.len != expected.len || memcmp(val.s, expected.s, val.len) != 0)
        fail("get.%d(%.*s) returned unexpected value %.*s (expected %.*s)\n",
             col, key.len, key.s, std::min(val.len, 40), val.s,
             std::min(expected.len, 40), expected.s);
    else
        ++checks_;
}

bool kvtest_client::get_sync(long ikey) {
    quick_istr key(ikey);
    Str val;
    return q_[0].run_get1(tree->table(), key.string(), 0, val, *ti_);
}

void kvtest_client::put(const Str &key, const Str &value) {
    while (failing)
        /* do nothing */;
    q_[0].run_replace(tree->table(), key, value, *ti_);
    if (ti_->logger()) // NB may block
        ti_->logger()->record(logcmd_replace, q_[0].query_times(), key, value);
}

void kvtest_client::put_col(const Str &key, int col, const Str &value) {
    while (failing)
        /* do nothing */;
#if !MASSTREE_ROW_TYPE_STR
    if (!kvo_)
        kvo_ = new_kvout(-1, 2048);
    Json req[2] = {Json(col), Json(String::make_stable(value))};
    (void) q_[0].run_put(tree->table(), key, &req[0], &req[2], *ti_);
    if (ti_->logger()) // NB may block
        ti_->logger()->record(logcmd_put, q_[0].query_times(), key,
                              &req[0], &req[2]);
#else
    (void) key, (void) col, (void) value;
    assert(0);
#endif
}

bool kvtest_client::remove_sync(long ikey) {
    quick_istr key(ikey);
    bool removed = q_[0].run_remove(tree->table(), key.string(), *ti_);
    if (removed && ti_->logger()) // NB may block
        ti_->logger()->record(logcmd_remove, q_[0].query_times(), key.string(), Str());
    return removed;
}

String kvtest_client::make_message(StringAccum &sa) const {
    const char *begin = sa.begin();
    while (begin != sa.end() && isspace((unsigned char) *begin))
        ++begin;
    String s = String(begin, sa.end());
    if (!s.empty() && s.back() != '\n')
        s += '\n';
    return s;
}

void kvtest_client::notice(const char *fmt, ...) {
    va_list val;
    va_start(val, fmt);
    String m = make_message(StringAccum().vsnprintf(500, fmt, val));
    va_end(val);
    if (m)
        fprintf(stderr, "%d: %s", ti_->index(), m.c_str());
}

void kvtest_client::fail(const char *fmt, ...) {
    static nodeversion32 failing_lock(false);
    static nodeversion32 fail_message_lock(false);
    static String fail_message;
    failing = 1;

    va_list val;
    va_start(val, fmt);
    String m = make_message(StringAccum().vsnprintf(500, fmt, val));
    va_end(val);
    if (!m)
        m = "unknown failure";

    fail_message_lock.lock();
    if (fail_message != m) {
        fail_message = m;
        fprintf(stderr, "%d: %s", ti_->index(), m.c_str());
    }
    fail_message_lock.unlock();

    if (doprint) {
        failing_lock.lock();
        fprintf(stdout, "%d: %s", ti_->index(), m.c_str());
        tree->print(stdout);
        fflush(stdout);
    }

    always_assert(0);
}

static void* testgo(void* x) {
    kvtest_client *kc = reinterpret_cast<kvtest_client*>(x);
    kc->ti_->pthread() = pthread_self();
    prepare_thread(kc->ti_);

    if (strcmp(kc->testname_, "rw1") == 0)
        kvtest_rw1(*kc);
    else if (strcmp(kc->testname_, "rw2") == 0)
        kvtest_rw2(*kc);
    else if (strcmp(kc->testname_, "rw3") == 0)
        kvtest_rw3(*kc);
    else if (strcmp(kc->testname_, "rw4") == 0)
        kvtest_rw4(*kc);
    else if (strcmp(kc->testname_, "rwsmall24") == 0)
        kvtest_rwsmall24(*kc);
    else if (strcmp(kc->testname_, "rwsep24") == 0)
        kvtest_rwsep24(*kc);
    else if (strcmp(kc->testname_, "palma") == 0)
        kvtest_palma(*kc);
    else if (strcmp(kc->testname_, "palmb") == 0)
        kvtest_palmb(*kc);
    else if (strcmp(kc->testname_, "rw16") == 0)
        kvtest_rw16(*kc);
    else if (strcmp(kc->testname_, "rw5") == 0
             || strcmp(kc->testname_, "rw1fixed") == 0)
        kvtest_rw1fixed(*kc);
    else if (strcmp(kc->testname_, "ycsbk") == 0)
        kvtest_ycsbk(*kc);
    else if (strcmp(kc->testname_, "wd1") == 0)
        kvtest_wd1(10000000, 1, *kc);
    else if (strcmp(kc->testname_, "wd1check") == 0)
        kvtest_wd1_check(10000000, 1, *kc);
    else if (strcmp(kc->testname_, "w1") == 0)
        kvtest_w1_seed(*kc, kvtest_first_seed + kc->id());
    else if (strcmp(kc->testname_, "r1") == 0)
        kvtest_r1_seed(*kc, kvtest_first_seed + kc->id());
    else if (strcmp(kc->testname_, "wcol1") == 0)
        kvtest_wcol1at(*kc, kc->id() % 24, kvtest_first_seed + kc->id() % 48, 5000000);
    else if (strcmp(kc->testname_, "rcol1") == 0)
        kvtest_rcol1at(*kc, kc->id() % 24, kvtest_first_seed + kc->id() % 48, 5000000);
    else
        kc->fail("unknown test '%s'", kc->testname_);
    return 0;
}

static const char * const kvstats_name[] = {
    "ops", "ops_per_sec", "puts", "gets", "scans", "puts_per_sec", "gets_per_sec", "scans_per_sec"
};

void runtest(const char *testname, int nthreads) {
    std::vector<kvtest_client> clients(nthreads, kvtest_client(testname));
    ::testthreads = nthreads;
    for (int i = 0; i < nthreads; ++i)
        clients[i].set_thread(threadinfo::make(threadinfo::TI_PROCESS, i));
    bzero((void *)timeout, sizeof(timeout));
    signal(SIGALRM, test_timeout);
    if (duration[0])
        xalarm(duration[0]);
    for (int i = 0; i < nthreads; ++i) {
        int r = pthread_create(&clients[i].ti_->pthread(), 0, testgo, &clients[i]);
        always_assert(r == 0);
    }
    for (int i = 0; i < nthreads; ++i)
        pthread_join(clients[i].ti_->pthread(), 0);

    kvstats kvs[arraysize(kvstats_name)];
    for (int i = 0; i < nthreads; ++i)
        for (int j = 0; j < (int) arraysize(kvstats_name); ++j)
            if (double x = clients[i].report_.get_d(kvstats_name[j]))
                kvs[j].add(x);
    for (int j = 0; j < (int) arraysize(kvstats_name); ++j)
        kvs[j].print_report(kvstats_name[j]);
}


struct conn {
    int fd;
    enum { inbufsz = 20 * 1024, inbufrefill = 16 * 1024 };

    conn(int s)
        : fd(s), inbuf_(new char[inbufsz]),
          inbufpos_(0), inbuflen_(0), kvout(new_kvout(s, 20 * 1024)),
          inbuftotal_(0) {
    }
    ~conn() {
        close(fd);
        free_kvout(kvout);
        delete[] inbuf_;
        for (char* x : oldinbuf_)
            delete[] x;
    }

    Json& receive() {
        while (!parser_.done() && check(2))
            inbufpos_ += parser_.consume(inbuf_ + inbufpos_,
                                         inbuflen_ - inbufpos_,
                                         String::make_stable(inbuf_, inbufsz));
        if (parser_.success() && parser_.result().is_a())
            parser_.reset();
        else
            parser_.result() = Json();
        return parser_.result();
    }

    int check(int tryhard) {
        if (inbufpos_ == inbuflen_ && tryhard)
            hard_check(tryhard);
        return inbuflen_ - inbufpos_;
    }

    uint64_t xposition() const {
        return inbuftotal_ + inbufpos_;
    }
    Str recent_string(uint64_t xposition) const {
        if (xposition - inbuftotal_ <= unsigned(inbufpos_))
            return Str(inbuf_ + (xposition - inbuftotal_),
                       inbuf_ + inbufpos_);
        else
            return Str();
    }

  private:
    char* inbuf_;
    int inbufpos_;
    int inbuflen_;
    std::vector<char*> oldinbuf_;
    msgpack::streaming_parser parser_;
  public:
    struct kvout *kvout;
  private:
    uint64_t inbuftotal_;

    void hard_check(int tryhard);
};

void conn::hard_check(int tryhard) {
    masstree_precondition(inbufpos_ == inbuflen_);
    if (parser_.empty()) {
        inbuftotal_ += inbufpos_;
        inbufpos_ = inbuflen_ = 0;
        for (auto x : oldinbuf_)
            delete[] x;
        oldinbuf_.clear();
    } else if (inbufpos_ == inbufsz) {
        oldinbuf_.push_back(inbuf_);
        inbuf_ = new char[inbufsz];
        inbuftotal_ += inbufpos_;
        inbufpos_ = inbuflen_ = 0;
    }
    if (tryhard == 1) {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        struct timeval tv = {0, 0};
        if (select(fd + 1, &rfds, NULL, NULL, &tv) <= 0)
            return;
    } else
        kvflush(kvout);

    ssize_t r = read(fd, inbuf_ + inbufpos_, inbufsz - inbufpos_);
    if (r != -1)
        inbuflen_ += r;
}

struct conninfo {
    int s;
    Json handshake;
};


/* main loop */

enum { clp_val_suffixdouble = Clp_ValFirstUser };
enum { opt_nolog = 1, opt_pin, opt_logdir, opt_port, opt_ckpdir, opt_duration,
       opt_test, opt_test_name, opt_threads, opt_cores,
       opt_print, opt_norun, opt_checkpoint, opt_limit, opt_epoch_interval };
static const Clp_Option options[] = {
    { "no-log", 0, opt_nolog, 0, 0 },
    { 0, 'n', opt_nolog, 0, 0 },
    { "no-run", 0, opt_norun, 0, 0 },
    { "pin", 'p', opt_pin, 0, Clp_Negate },
    { "logdir", 0, opt_logdir, Clp_ValString, 0 },
    { "ld", 0, opt_logdir, Clp_ValString, 0 },
    { "checkpoint", 'c', opt_checkpoint, Clp_ValDouble, Clp_Optional | Clp_Negate },
    { "ckp", 0, opt_checkpoint, Clp_ValDouble, Clp_Optional | Clp_Negate },
    { "ckpdir", 0, opt_ckpdir, Clp_ValString, 0 },
    { "ckdir", 0, opt_ckpdir, Clp_ValString, 0 },
    { "cd", 0, opt_ckpdir, Clp_ValString, 0 },
    { "port", 0, opt_port, Clp_ValInt, 0 },
    { "duration", 'd', opt_duration, Clp_ValDouble, 0 },
    { "limit", 'l', opt_limit, clp_val_suffixdouble, 0 },
    { "test", 0, opt_test, Clp_ValString, 0 },
    { "test-rw1", 0, opt_test_name, 0, 0 },
    { "test-rw2", 0, opt_test_name, 0, 0 },
    { "test-rw3", 0, opt_test_name, 0, 0 },
    { "test-rw4", 0, opt_test_name, 0, 0 },
    { "test-rw5", 0, opt_test_name, 0, 0 },
    { "test-rw16", 0, opt_test_name, 0, 0 },
    { "test-palm", 0, opt_test_name, 0, 0 },
    { "test-ycsbk", 0, opt_test_name, 0, 0 },
    { "test-rw1fixed", 0, opt_test_name, 0, 0 },
    { "threads", 'j', opt_threads, Clp_ValInt, 0 },
    { "cores", 0, opt_cores, Clp_ValString, 0 },
    { "print", 0, opt_print, 0, Clp_Negate },
    { "epoch-interval", 0, opt_epoch_interval, Clp_ValDouble, 0 }
};

int
main1(int argc, char *argv[])
{
  using std::swap;
  int s, ret, yes = 1, i = 1, firstcore = -1, corestride = 1;
  const char *dotest = 0;
  nlogger = tcpthreads = udpthreads = nckthreads = sysconf(_SC_NPROCESSORS_ONLN);
  Clp_Parser *clp = Clp_NewParser(argc, argv, (int) arraysize(options), options);
  Clp_AddType(clp, clp_val_suffixdouble, Clp_DisallowOptions, clp_parse_suffixdouble, 0);
  int opt;
  double epoch_interval_ms = 1000;
  while ((opt = Clp_Next(clp)) >= 0) {
      switch (opt) {
      case opt_nolog:
          logging = false;
          break;
      case opt_pin:
          pinthreads = !clp->negated;
          break;
      case opt_threads:
          nlogger = tcpthreads = udpthreads = nckthreads = clp->val.i;
          break;
      case opt_logdir: {
          const char *s = strtok((char *) clp->vstr, ",");
          for (; s; s = strtok(NULL, ","))
              logdirs.push_back(s);
          break;
      }
      case opt_ckpdir: {
          const char *s = strtok((char *) clp->vstr, ",");
          for (; s; s = strtok(NULL, ","))
              ckpdirs.push_back(s);
          break;
      }
      case opt_checkpoint:
          if (clp->negated || (clp->have_val && clp->val.d <= 0))
              checkpoint_interval = -1;
          else if (clp->have_val)
              checkpoint_interval = clp->val.d;
          else
              checkpoint_interval = 30;
          break;
      case opt_port:
          port = clp->val.i;
          break;
      case opt_duration:
          duration[0] = clp->val.d;
          break;
      case opt_limit:
          test_limit = (uint64_t) clp->val.d;
          break;
      case opt_test:
          dotest = clp->vstr;
          break;
      case opt_test_name:
          dotest = clp->option->long_name + 5;
          break;
      case opt_print:
          doprint = !clp->negated;
          break;
      case opt_cores:
          if (firstcore >= 0 || cores.size() > 0) {
              Clp_OptionError(clp, "%<%O%> already given");
              exit(EXIT_FAILURE);
          } else {
              const char *plus = strchr(clp->vstr, '+');
              Json ij = Json::parse(clp->vstr),
                  aj = Json::parse(String("[") + String(clp->vstr) + String("]")),
                  pj1 = Json::parse(plus ? String(clp->vstr, plus) : "x"),
                  pj2 = Json::parse(plus ? String(plus + 1) : "x");
              for (int i = 0; aj && i < aj.size(); ++i)
                  if (!aj[i].is_int() || aj[i].to_i() < 0)
                      aj = Json();
              if (ij && ij.is_int() && ij.to_i() >= 0)
                  firstcore = ij.to_i(), corestride = 1;
              else if (pj1 && pj2 && pj1.is_int() && pj1.to_i() >= 0 && pj2.is_int())
                  firstcore = pj1.to_i(), corestride = pj2.to_i();
              else if (aj) {
                  for (int i = 0; i < aj.size(); ++i)
                      cores.push_back(aj[i].to_i());
              } else {
                  Clp_OptionError(clp, "bad %<%O%>, expected %<CORE1%>, %<CORE1+STRIDE%>, or %<CORE1,CORE2,...%>");
                  exit(EXIT_FAILURE);
              }
          }
          break;
      case opt_norun:
          recovery_only = true;
          break;
      case opt_epoch_interval:
	epoch_interval_ms = clp->val.d;
	break;
      default:
          fprintf(stderr, "Usage: mtd [-np] [--ld dir1[,dir2,...]] [--cd dir1[,dir2,...]]\n");
          exit(EXIT_FAILURE);
      }
  }
  Clp_DeleteParser(clp);
  if (logdirs.empty())
      logdirs.push_back(".");
  if (ckpdirs.empty())
      ckpdirs.push_back(".");
  if (firstcore < 0)
      firstcore = cores.size() ? cores.back() + 1 : 0;
  for (; (int) cores.size() < udpthreads; firstcore += corestride)
      cores.push_back(firstcore);

  // for -pg profiling
  signal(SIGINT, catchint);

  // log epoch starts at 1
  global_log_epoch = 1;
  global_wake_epoch = 0;
  log_epoch_interval.tv_sec = 0;
  log_epoch_interval.tv_usec = 200000;

  // set a timer for incrementing the global epoch
  if (!dotest) {
      if (!epoch_interval_ms) {
	  printf("WARNING: epoch interval is 0, it means no GC is executed\n");
      } else {
	  signal(SIGALRM, epochinc);
	  struct itimerval etimer;
	  etimer.it_interval.tv_sec = epoch_interval_ms / 1000;
	  etimer.it_interval.tv_usec = fmod(epoch_interval_ms, 1000) * 1000;
	  etimer.it_value.tv_sec = epoch_interval_ms / 1000;
	  etimer.it_value.tv_usec = fmod(epoch_interval_ms, 1000) * 1000;
	  ret = setitimer(ITIMER_REAL, &etimer, NULL);
	  always_assert(ret == 0);
      }
  }

  // for parallel recovery
  ret = pthread_cond_init(&rec_cond, 0);
  always_assert(ret == 0);
  ret = pthread_mutex_init(&rec_mu, 0);
  always_assert(ret == 0);

  // for waking up the checkpoint thread
  ret = pthread_cond_init(&checkpoint_cond, 0);
  always_assert(ret == 0);
  ret = pthread_mutex_init(&checkpoint_mu, 0);
  always_assert(ret == 0);

  threadinfo *main_ti = threadinfo::make(threadinfo::TI_MAIN, -1);
  main_ti->pthread() = pthread_self();

  initial_timestamp = timestamp();
  tree = new Masstree::default_table;
  tree->initialize(*main_ti);
  printf("%s, %s, pin-threads %s, ", tree->name(), row_type::name(),
         pinthreads ? "enabled" : "disabled");
  if(logging){
    printf("logging enabled\n");
    log_init();
    recover(main_ti);
  } else {
    printf("logging disabled\n");
  }

  // UDP threads, each with its own port.
  if (udpthreads == 0)
      printf("0 udp threads\n");
  else if (udpthreads == 1)
      printf("1 udp thread (port %d)\n", port);
  else
      printf("%d udp threads (ports %d-%d)\n", udpthreads, port, port + udpthreads - 1);
  for(i = 0; i < udpthreads; i++){
    threadinfo *ti = threadinfo::make(threadinfo::TI_PROCESS, i);
    ret = pthread_create(&ti->pthread(), 0, udp_threadfunc, ti);
    always_assert(ret == 0);
  }

  if (dotest) {
      if (strcmp(dotest, "palm") == 0) {
        runtest("palma", 1);
        runtest("palmb", tcpthreads);
      } else
        runtest(dotest, tcpthreads);
      tree->stats(stderr);
      if (doprint)
          tree->print(stdout);
      exit(0);
  }

  // TCP socket and threads

  s = socket(AF_INET, SOCK_STREAM, 0);
  always_assert(s >= 0);
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));

  struct sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons(port);
  ret = bind(s, (struct sockaddr *) &sin, sizeof(sin));
  if (ret < 0) {
      perror("bind");
      exit(EXIT_FAILURE);
  }

  ret = listen(s, 100);
  if (ret < 0) {
      perror("listen");
      exit(EXIT_FAILURE);
  }

  threadinfo **tcpti = new threadinfo *[tcpthreads];
  tcp_thread_pipes = new int[tcpthreads * 2];
  printf("%d tcp threads (port %d)\n", tcpthreads, port);
  for(i = 0; i < tcpthreads; i++){
    threadinfo *ti = threadinfo::make(threadinfo::TI_PROCESS, i);
    ret = pipe(&tcp_thread_pipes[i * 2]);
    always_assert(ret == 0);
    ret = pthread_create(&ti->pthread(), 0, tcp_threadfunc, ti);
    always_assert(ret == 0);
    tcpti[i] = ti;
  }
  // Create a canceling thread.
  ret = pipe(quit_pipe);
  always_assert(ret == 0);
  pthread_t canceling_tid;
  ret = pthread_create(&canceling_tid, NULL, canceling, NULL);
  always_assert(ret == 0);

  static int next = 0;
  while(1){
    int s1;
    struct sockaddr_in sin1;
    socklen_t sinlen = sizeof(sin1);

    bzero(&sin1, sizeof(sin1));
    s1 = accept(s, (struct sockaddr *) &sin1, &sinlen);
    always_assert(s1 >= 0);
    setsockopt(s1, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));

    // Complete handshake.
    char buf[BUFSIZ];
    ssize_t nr = read(s1, buf, BUFSIZ);
    if (nr == -1) {
        perror("read");
    kill_connection:
        close(s1);
        continue;
    }

    msgpack::streaming_parser sp;
    if (nr == 0 || sp.consume(buf, nr) != (size_t) nr
        || !sp.result().is_a() || sp.result().size() < 2
        || !sp.result()[1].is_i() || sp.result()[1].as_i() != Cmd_Handshake) {
        fprintf(stderr, "failed handshake\n");
        goto kill_connection;
    }

    int target_core = -1;
    if (sp.result().size() >= 3 && sp.result()[2].is_o()
        && sp.result()[2]["core"].is_i())
        target_core = sp.result()[2]["core"].as_i();
    if (target_core < 0 || target_core >= tcpthreads) {
        target_core = next % tcpthreads;
        ++next;
    }

    conninfo* ci = new conninfo;
    ci->s = s1;
    swap(ci->handshake, sp.result());

    ssize_t w = write(tcp_thread_pipes[2*target_core + 1], &ci, sizeof(ci));
    always_assert((size_t) w == sizeof(ci));
  }
}

void
catchint(int)
{
    go_quit = 1;
    char cmd = 0;
    // Does not matter if the write fails (when the pipe is full)
    int r = write(quit_pipe[1], &cmd, sizeof(cmd));
    (void)r;
}

inline const char *threadtype(int type) {
  switch (type) {
    case threadinfo::TI_MAIN:
      return "main";
    case threadinfo::TI_PROCESS:
      return "process";
    case threadinfo::TI_LOG:
      return "log";
    case threadinfo::TI_CHECKPOINT:
      return "checkpoint";
    default:
      always_assert(0 && "Unknown threadtype");
      break;
  };
}

void *
canceling(void *)
{
    char cmd;
    int r = read(quit_pipe[0], &cmd, sizeof(cmd));
    (void) r;
    assert(r == sizeof(cmd) && cmd == 0);
    // Cancel wake up checkpointing threads
    pthread_mutex_lock(&checkpoint_mu);
    pthread_cond_signal(&checkpoint_cond);
    pthread_mutex_unlock(&checkpoint_mu);

    fprintf(stderr, "\n");
    // cancel outstanding threads. Checkpointing threads will exit safely
    // when the checkpointing thread 0 sees go_quit, and don't need cancel
    for (threadinfo *ti = threadinfo::allthreads; ti; ti = ti->next())
        if (ti->purpose() != threadinfo::TI_MAIN
            && ti->purpose() != threadinfo::TI_CHECKPOINT) {
            int r = pthread_cancel(ti->pthread());
            always_assert(r == 0);
        }

    // join canceled threads
    for (threadinfo *ti = threadinfo::allthreads; ti; ti = ti->next())
        if (ti->purpose() != threadinfo::TI_MAIN) {
            fprintf(stderr, "joining thread %s:%d\n",
                    threadtype(ti->purpose()), ti->index());
            int r = pthread_join(ti->pthread(), 0);
            always_assert(r == 0);
        }
    tree->stats(stderr);
    exit(0);
}

void
epochinc(int)
{
    globalepoch.store(globalepoch.load() + 2);
    active_epoch.store(threadinfo::min_active_epoch());
}

// Return 1 if success, -1 if I/O error or protocol unmatch
int handshake(Json& request, threadinfo& ti) {
    always_assert(request.is_a() && request.size() >= 2
                  && request[1].is_i() && request[1].as_i() == Cmd_Handshake
                  && (request.size() == 2 || request[2].is_o()));
    if (request.size() >= 2
        && request[2]["maxkeylen"].is_i()
        && request[2]["maxkeylen"].as_i() > MASSTREE_MAXKEYLEN) {
        request[2] = false;
        request[3] = "bad maxkeylen";
        request.resize(4);
    } else {
        request[2] = true;
        request[3] = ti.index();
        request[4] = row_type::name();
        request.resize(5);
    }
    request[1] = Cmd_Handshake + 1;
    return request[2].as_b() ? 1 : -1;
}

// execute command, return result.
int onego(query<row_type>& q, Json& request, Str request_str, threadinfo& ti) {
    int command = request[1].as_i();
    if (command == Cmd_Checkpoint) {
        // force checkpoint
        pthread_mutex_lock(&checkpoint_mu);
        pthread_cond_broadcast(&checkpoint_cond);
        pthread_mutex_unlock(&checkpoint_mu);
        request.resize(2);
    } else if (command == Cmd_Get) {
        //printf("GET \n");
        q.run_get(tree->table(), request, ti);
    } else if (command == Cmd_Put && request.size() > 3
               && (request.size() % 2) == 1) { // insert or update
        Str key(request[2].as_s());
        const Json* req = request.array_data() + 3;
        const Json* end_req = request.end_array_data();
        request[2] = q.run_put(tree->table(), request[2].as_s(),
                               req, end_req, ti);
        if (ti.logger() && request_str) {
            // use the client's parsed version of the request
            msgpack::parser mp(request_str.data());
            mp.skip_array_size().skip_primitives(3);
            ti.logger()->record(logcmd_put, q.query_times(), key, Str(mp.position(), request_str.end()));
        } else if (ti.logger())
            ti.logger()->record(logcmd_put, q.query_times(), key, req, end_req);
        request.resize(3);
    } else if (command == Cmd_Replace) { // insert or update
        //printf("PUT \n");
        Str key(request[2].as_s()), value(request[3].as_s());
        request[2] = q.run_replace(tree->table(), key, value, ti);
        if (ti.logger()) // NB may block
            ti.logger()->record(logcmd_replace, q.query_times(), key, value);
        request.resize(3);
    } else if (command == Cmd_Remove) { // remove
        Str key(request[2].as_s());
        bool removed = q.run_remove(tree->table(), key, ti);
        if (removed && ti.logger()) // NB may block
            ti.logger()->record(logcmd_remove, q.query_times(), key, Str());
        request[2] = removed;
        request.resize(3);
    } else if (command == Cmd_Scan) {
        //printf("SCAN \n");
        q.run_scan(tree->table(), request, ti);
    } else {
        request[1] = -1;
        request.resize(2);
        return -1;
    }
    request[1] = command + 1;
    return 1;
}

#if HAVE_SYS_EPOLL_H
struct tcpfds {
    int epollfd;

    tcpfds(int pipefd) {
        epollfd = epoll_create(10);
        if (epollfd < 0) {
            perror("epoll_create");
            exit(EXIT_FAILURE);
        }
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.ptr = (void *) 1;
        int r = epoll_ctl(epollfd, EPOLL_CTL_ADD, pipefd, &ev);
        always_assert(r == 0);
    }

    enum { max_events = 100 };
    typedef struct epoll_event eventset[max_events];
    int wait(eventset &es) {
        return epoll_wait(epollfd, es, max_events, -1);
    }

    conn *event_conn(eventset &es, int i) const {
        return (conn *) es[i].data.ptr;
    }

    void add(int fd, conn *c) {
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.ptr = c;
        int r = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
        always_assert(r == 0);
    }

    void remove(int fd) {
        int r = epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
        always_assert(r == 0);
    }
};
#else
class tcpfds {
    int nfds_;
    fd_set rfds_;
    std::vector<conn *> conns_;

  public:
    tcpfds(int pipefd)
        : nfds_(pipefd + 1) {
        always_assert(pipefd < FD_SETSIZE);
        FD_ZERO(&rfds_);
        FD_SET(pipefd, &rfds_);
        conns_.resize(nfds_, 0);
        conns_[pipefd] = (conn *) 1;
    }

    typedef fd_set eventset;
    int wait(eventset &es) {
        es = rfds_;
        int r = select(nfds_, &es, 0, 0, 0);
        return r > 0 ? nfds_ : r;
    }

    conn *event_conn(eventset &es, int i) const {
        return FD_ISSET(i, &es) ? conns_[i] : 0;
    }

    void add(int fd, conn *c) {
        always_assert(fd < FD_SETSIZE);
        FD_SET(fd, &rfds_);
        if (fd >= nfds_) {
            nfds_ = fd + 1;
            conns_.resize(nfds_, 0);
        }
        conns_[fd] = c;
    }

    void remove(int fd) {
        always_assert(fd < FD_SETSIZE);
        FD_CLR(fd, &rfds_);
        if (fd == nfds_ - 1) {
            while (nfds_ > 0 && !FD_ISSET(nfds_ - 1, &rfds_))
                --nfds_;
        }
    }
};
#endif

void prepare_thread(threadinfo *ti) {
#if __linux__
    if (pinthreads) {
        cpu_set_t cs;
        CPU_ZERO(&cs);
        CPU_SET(cores[ti->index()], &cs);
        always_assert(sched_setaffinity(0, sizeof(cs), &cs) == 0);
    }
#else
    always_assert(!pinthreads && "pinthreads not supported\n");
#endif
    if (logging)
        ti->set_logger(&logs->log(ti->index() % nlogger));
}

void* tcp_threadfunc(void* x) {
    threadinfo* ti = reinterpret_cast<threadinfo*>(x);
    ti->pthread() = pthread_self();
    prepare_thread(ti);

    int myfd = tcp_thread_pipes[2 * ti->index()];
    tcpfds sloop(myfd);
    tcpfds::eventset events;
    std::deque<conn*> ready;
    query<row_type> q;

    while (1) {
        int nev = sloop.wait(events);
        for (int i = 0; i < nev; i++)
            if (conn *c = sloop.event_conn(events, i))
                ready.push_back(c);

        while (!ready.empty()) {
            conn* c = ready.front();
            ready.pop_front();

            if (c == (conn *) 1) {
                // new connections
#define MAX_NEWCONN 100
                conninfo* ci[MAX_NEWCONN];
                ssize_t len = read(myfd, ci, sizeof(ci));
                always_assert(len > 0 && len % sizeof(int) == 0);
                for (int j = 0; j * sizeof(*ci) < (size_t) len; ++j) {
                    struct conn *c = new conn(ci[j]->s);
                    sloop.add(c->fd, c);
                    int ret = handshake(ci[j]->handshake, *ti);
                    msgpack::unparse(*c->kvout, ci[j]->handshake);
                    kvflush(c->kvout);
                    if (ret < 0) {
                        sloop.remove(c->fd);
                        delete c;
                    }
                    delete ci[j];
                }
            } else if (c) {
                // Should not block as suggested by epoll
                uint64_t xposition = c->xposition();
                Json& request = c->receive();
                int ret;
                if (unlikely(!request))
                    goto closed;
                ti->rcu_start();
                ret = onego(q, request, c->recent_string(xposition), *ti);
                ti->rcu_stop();
                msgpack::unparse(*c->kvout, request);
                request.clear();
                if (likely(ret >= 0)) {
                    if (c->check(0))
                        ready.push_back(c);
                    else
                        kvflush(c->kvout);
                    continue;
                }
                printf("socket read error\n");
            closed:
                kvflush(c->kvout);
                sloop.remove(c->fd);
                delete c;
            }
        }
    }
    return 0;
}

// serve a client udp socket, in a dedicated thread
void* udp_threadfunc(void* x) {
  threadinfo* ti = reinterpret_cast<threadinfo*>(x);
  ti->pthread() = pthread_self();
  prepare_thread(ti);

  struct sockaddr_in sin;
  bzero(&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port + ti->index());

  int s = socket(AF_INET, SOCK_DGRAM, 0);
  always_assert(s >= 0);
  int ret = bind(s, (struct sockaddr *) &sin, sizeof(sin));
  always_assert(ret == 0 && "bind failed");
  int sobuflen = 512*1024;
  setsockopt(s, SOL_SOCKET, SO_RCVBUF, &sobuflen, sizeof(sobuflen));

  String buf = String::make_uninitialized(4096);
  struct kvout *kvout = new_bufkvout();
  msgpack::streaming_parser parser;
  StringAccum sa;

  query<row_type> q;
  while(1){
    struct sockaddr_in sin;
    socklen_t sinlen = sizeof(sin);
    ssize_t cc = recvfrom(s, const_cast<char*>(buf.data()), buf.length(),
                          0, (struct sockaddr *) &sin, &sinlen);
    if(cc < 0){
      perror("udpgo read");
      exit(EXIT_FAILURE);
    }
    kvout_reset(kvout);

    parser.reset();
    unsigned consumed = parser.consume(buf.data(), buf.length(), buf);

    // Fail if we received a partial request
    if (parser.success() && parser.result().is_a()) {
        ti->rcu_start();
        if (onego(q, parser.result(), Str(buf.data(), consumed), *ti) >= 0) {
            sa.clear();
            msgpack::unparser<StringAccum> cu(sa);
            cu << parser.result();
            cc = sendto(s, sa.data(), sa.length(), 0,
                        (struct sockaddr*) &sin, sinlen);
            always_assert(cc == (ssize_t) sa.length());
        }
        ti->rcu_stop();
    } else
      printf("onego failed\n");
  }
  return 0;
}

static String log_filename(const char* logdir, int logindex) {
    struct stat sb;
    int r = stat(logdir, &sb);
    if (r < 0 && errno == ENOENT) {
        r = mkdir(logdir, 0777);
        if (r < 0) {
            fprintf(stderr, "%s: %s\n", logdir, strerror(errno));
            always_assert(0);
        }
    }

    StringAccum sa;
    sa.snprintf(strlen(logdir) + 24, "%s/kvd-log-%d", logdir, logindex);
    return sa.take_string();
}

void log_init() {
  int ret, i;

  logs = logset::make(nlogger);
  for (i = 0; i < nlogger; i++)
      logs->log(i).initialize(log_filename(logdirs[i % logdirs.size()], i));

  cks = (ckstate *)malloc(sizeof(ckstate) * nckthreads);
  for (i = 0; i < nckthreads; i++) {
    threadinfo *ti = threadinfo::make(threadinfo::TI_CHECKPOINT, i);
    cks[i].state = CKState_Uninit;
    cks[i].ti = ti;
    ret = pthread_create(&ti->pthread(), 0, conc_checkpointer, ti);
    always_assert(ret == 0);
  }
}

// read a checkpoint, insert key/value pairs into tree.
// must be followed by a read of the log!
// since checkpoint is not consistent
// with any one point in time.
// returns the timestamp of the first log record that needs
// to come from the log.
kvepoch_t read_checkpoint(threadinfo *ti, const char *path) {
    double t0 = now();

    int fd = open(path, 0);
    if(fd < 0){
        printf("no %s\n", path);
        return 0;
    }
    struct stat sb;
    int ret = fstat(fd, &sb);
    always_assert(ret == 0);
    char *p = (char *) mmap(0, sb.st_size, PROT_READ, MAP_FILE|MAP_PRIVATE, fd, 0);
    always_assert(p != MAP_FAILED);
    close(fd);

    msgpack::parser par(String::make_stable(p, sb.st_size));
    Json j;
    par >> j;
    std::cerr << j << "\n";
    always_assert(j["generation"].is_i() && j["size"].is_i());
    uint64_t gen = j["generation"].as_i();
    uint64_t n = j["size"].as_i();
    printf("reading checkpoint with %" PRIu64 " nodes\n", n);

    // read data
    for (uint64_t i = 0; i != n; ++i)
        ckstate::insert(tree->table(), par, *ti);

    munmap(p, sb.st_size);
    double t1 = now();
    printf("%.1f MB, %.2f sec, %.1f MB/sec\n",
           sb.st_size / 1000000.0,
           t1 - t0,
           (sb.st_size / 1000000.0) / (t1 - t0));
    return gen;
}

void
waituntilphase(int phase)
{
  always_assert(pthread_mutex_lock(&rec_mu) == 0);
  while (rec_state != phase)
    always_assert(pthread_cond_wait(&rec_cond, &rec_mu) == 0);
  always_assert(pthread_mutex_unlock(&rec_mu) == 0);
}

void
inactive(void)
{
  always_assert(pthread_mutex_lock(&rec_mu) == 0);
  rec_nactive --;
  always_assert(pthread_cond_broadcast(&rec_cond) == 0);
  always_assert(pthread_mutex_unlock(&rec_mu) == 0);
}

void recovercheckpoint(threadinfo *ti) {
    waituntilphase(REC_CKP);
    char path[256];
    sprintf(path, "%s/kvd-ckp-%" PRId64 "-%d",
            ckpdirs[ti->index() % ckpdirs.size()],
            ckp_gen.value(), ti->index());
    kvepoch_t gen = read_checkpoint(ti, path);
    always_assert(ckp_gen == gen);
    inactive();
}

void
recphase(int nactive, int state)
{
  rec_nactive = nactive;
  rec_state = state;
  always_assert(pthread_cond_broadcast(&rec_cond) == 0);
  while (rec_nactive)
    always_assert(pthread_cond_wait(&rec_cond, &rec_mu) == 0);
}

// read the checkpoint file.
// read each log file.
// insert will ignore attempts to update with timestamps
// less than what was in the entry from the checkpoint file.
// so we don't have to do an explicit merge by time of the log files.
void
recover(threadinfo *)
{
  recovering = true;
  // XXX: discard temporary checkpoint and ckp-gen files generated before crash

  // get the generation of the checkpoint from ckp-gen, if any
  char path[256];
  sprintf(path, "%s/kvd-ckp-gen", ckpdirs[0]);
  ckp_gen = 0;
  rec_ckp_min_epoch = rec_ckp_max_epoch = 0;
  int fd = open(path, O_RDONLY);
  if (fd >= 0) {
      Json ckpj = Json::parse(read_file_contents(fd));
      close(fd);
      if (ckpj && ckpj["kvdb_checkpoint"] && ckpj["generation"].is_number()) {
          ckp_gen = ckpj["generation"].to_u64();
          rec_ckp_min_epoch = ckpj["min_epoch"].to_u64();
          rec_ckp_max_epoch = ckpj["max_epoch"].to_u64();
          printf("recover from checkpoint %" PRIu64 " [%" PRIu64 ", %" PRIu64 "]\n", ckp_gen.value(), rec_ckp_min_epoch.value(), rec_ckp_max_epoch.value());
      }
  } else {
    printf("no %s\n", path);
  }
  always_assert(pthread_mutex_lock(&rec_mu) == 0);

  // recover from checkpoint, and set timestamp of the checkpoint
  recphase(nckthreads, REC_CKP);

  // find minimum maximum timestamp of entries in each log
  rec_log_infos = new logreplay::info_type[nlogger];
  recphase(nlogger, REC_LOG_TS);

  // replay log entries, remove inconsistent entries, and append
  // an empty log entry with minimum timestamp

  // calculate log range

  // Maximum epoch seen in the union of the logs and the checkpoint. (We
  // don't commit a checkpoint until all logs are flushed past the
  // checkpoint's max_epoch.)
  kvepoch_t max_epoch = rec_ckp_max_epoch;
  if (max_epoch)
      max_epoch = max_epoch.next_nonzero();
  for (logreplay::info_type *it = rec_log_infos;
       it != rec_log_infos + nlogger; ++it)
      if (it->last_epoch
          && (!max_epoch || max_epoch < it->last_epoch))
          max_epoch = it->last_epoch;

  // Maximum first_epoch seen in the logs. Full log information is not
  // available for epochs before max_first_epoch.
  kvepoch_t max_first_epoch = 0;
  for (logreplay::info_type *it = rec_log_infos;
       it != rec_log_infos + nlogger; ++it)
      if (it->first_epoch
          && (!max_first_epoch || max_first_epoch < it->first_epoch))
          max_first_epoch = it->first_epoch;

  // Maximum epoch of all logged wake commands.
  kvepoch_t max_wake_epoch = 0;
  for (logreplay::info_type *it = rec_log_infos;
       it != rec_log_infos + nlogger; ++it)
      if (it->wake_epoch
          && (!max_wake_epoch || max_wake_epoch < it->wake_epoch))
          max_wake_epoch = it->wake_epoch;

  // Minimum last_epoch seen in QUIESCENT logs.
  kvepoch_t min_quiescent_last_epoch = 0;
  for (logreplay::info_type *it = rec_log_infos;
       it != rec_log_infos + nlogger; ++it)
      if (it->quiescent
          && (!min_quiescent_last_epoch || min_quiescent_last_epoch > it->last_epoch))
          min_quiescent_last_epoch = it->last_epoch;

  // If max_wake_epoch && min_quiescent_last_epoch <= max_wake_epoch, then a
  // wake command was missed by at least one quiescent log. We can't replay
  // anything at or beyond the minimum missed wake epoch. So record, for
  // each log, the minimum wake command that at least one quiescent thread
  // missed.
  if (max_wake_epoch && min_quiescent_last_epoch <= max_wake_epoch)
      rec_replay_min_quiescent_last_epoch = min_quiescent_last_epoch;
  else
      rec_replay_min_quiescent_last_epoch = 0;
  recphase(nlogger, REC_LOG_ANALYZE_WAKE);

  // Calculate upper bound of epochs to replay.
  // This is the minimum of min_post_quiescent_wake_epoch (if any) and the
  // last_epoch of all non-quiescent logs.
  rec_replay_max_epoch = max_epoch;
  for (logreplay::info_type *it = rec_log_infos;
       it != rec_log_infos + nlogger; ++it) {
      if (!it->quiescent
          && it->last_epoch
          && it->last_epoch < rec_replay_max_epoch)
          rec_replay_max_epoch = it->last_epoch;
      if (it->min_post_quiescent_wake_epoch
          && it->min_post_quiescent_wake_epoch < rec_replay_max_epoch)
          rec_replay_max_epoch = it->min_post_quiescent_wake_epoch;
  }

  // Calculate lower bound of epochs to replay.
  rec_replay_min_epoch = rec_ckp_min_epoch;
  // XXX what about max_first_epoch?

  // Checks.
  if (rec_ckp_min_epoch) {
      always_assert(rec_ckp_min_epoch > max_first_epoch);
      always_assert(rec_ckp_min_epoch < rec_replay_max_epoch);
      always_assert(rec_ckp_max_epoch < rec_replay_max_epoch);
      fprintf(stderr, "replay [%" PRIu64 ",%" PRIu64 ") from [%" PRIu64 ",%" PRIu64 ") into ckp [%" PRIu64 ",%" PRIu64 "]\n",
              rec_replay_min_epoch.value(), rec_replay_max_epoch.value(),
              max_first_epoch.value(), max_epoch.value(),
              rec_ckp_min_epoch.value(), rec_ckp_max_epoch.value());
  }

  // Actually replay.
  delete[] rec_log_infos;
  rec_log_infos = 0;
  recphase(nlogger, REC_LOG_REPLAY);

  // done recovering
  recphase(0, REC_DONE);
#if !NDEBUG
  // check that all delta markers have been recycled (leaving only remove
  // markers and real values)
  uint64_t deltas_created = 0, deltas_removed = 0;
  for (threadinfo *ti = threadinfo::allthreads; ti; ti = ti->next()) {
      deltas_created += ti->counter(tc_replay_create_delta);
      deltas_removed += ti->counter(tc_replay_remove_delta);
  }
  if (deltas_created)
      fprintf(stderr, "deltas created: %" PRIu64 ", removed: %" PRIu64 "\n", deltas_created, deltas_removed);
  always_assert(deltas_created == deltas_removed);
#endif

  global_log_epoch = rec_replay_max_epoch.next_nonzero();

  always_assert(pthread_mutex_unlock(&rec_mu) == 0);
  recovering = false;
  if (recovery_only)
      exit(0);
}

void
writecheckpoint(const char *path, ckstate *c, double t0)
{
  double t1 = now();
  printf("memory phase: %" PRIu64 " nodes, %.2f sec\n", c->count, t1 - t0);

  int fd = creat(path, 0666);
  always_assert(fd >= 0);

  // checkpoint file format, all msgpack:
  //   {"generation": generation, "size": size, ...}
  //   then `size` triples of key (string), timestmap (int), value (whatever)
  Json j = Json().set("generation", ckp_gen.value())
      .set("size", c->count)
      .set("firstkey", c->startkey);
  StringAccum sa;
  msgpack::unparse(sa, j);
  checked_write(fd, sa.data(), sa.length());
  checked_write(fd, c->vals->buf, c->vals->n);

  int ret = fsync(fd);
  always_assert(ret == 0);
  ret = close(fd);
  always_assert(ret == 0);

  double t2 = now();
  c->bytes = c->vals->n;
  printf("file phase (%s): %" PRIu64 " bytes, %.2f sec, %.1f MB/sec\n",
         path,
         c->bytes,
         t2 - t1,
         (c->bytes / 1000000.0) / (t2 - t1));
}

void
conc_filecheckpoint(threadinfo *ti)
{
    ckstate *c = &cks[ti->index()];
    c->vals = new_bufkvout();
    double t0 = now();
    tree->table().scan(c->startkey, true, *c, *ti);
    char path[256];
    sprintf(path, "%s/kvd-ckp-%" PRId64 "-%d",
            ckpdirs[ti->index() % ckpdirs.size()],
            ckp_gen.value(), ti->index());
    writecheckpoint(path, c, t0);
    c->count = 0;
    free(c->vals);
}

static Json
prepare_checkpoint(kvepoch_t min_epoch, int nckthreads, const Str *pv)
{
    Json j;
    j.set("kvdb_checkpoint", true)
        .set("min_epoch", min_epoch.value())
        .set("max_epoch", global_log_epoch.value())
        .set("generation", ckp_gen.value())
        .set("nckthreads", nckthreads);

    Json pvj;
    for (int i = 1; i < nckthreads; ++i)
        pvj.push_back(Json::make_string(pv[i].s, pv[i].len));
    j.set("pivots", pvj);

    return j;
}

static void
commit_checkpoint(Json ckpj)
{
    // atomically commit a set of checkpoint files by incrementing
    // the checkpoint generation on disk
    char path[256];
    sprintf(path, "%s/kvd-ckp-gen", ckpdirs[0]);
    int r = atomic_write_file_contents(path, ckpj.unparse());
    always_assert(r == 0);
    fprintf(stderr, "kvd-ckp-%" PRIu64 " [%s,%s]: committed\n",
            ckp_gen.value(), ckpj["min_epoch"].to_s().c_str(),
            ckpj["max_epoch"].to_s().c_str());

    // delete old checkpoint files
    for (int i = 0; i < nckthreads; i++) {
        char path[256];
        sprintf(path, "%s/kvd-ckp-%" PRId64 "-%d",
                ckpdirs[i % ckpdirs.size()],
                ckp_gen.value() - 1, i);
        unlink(path);
    }
}

static kvepoch_t
max_flushed_epoch()
{
    kvepoch_t mfe = 0, ge = global_log_epoch;
    for (int i = 0; i < nlogger; ++i) {
        loginfo& log = logs->log(i);
        kvepoch_t fe = log.quiescent() ? ge : log.flushed_epoch();
        if (!mfe || fe < mfe)
            mfe = fe;
    }
    return mfe;
}

// concurrent periodic checkpoint
void* conc_checkpointer(void* x) {
  threadinfo* ti = reinterpret_cast<threadinfo*>(x);
  ti->pthread() = pthread_self();
  recovercheckpoint(ti);
  ckstate *c = &cks[ti->index()];
  c->count = 0;
  pthread_cond_init(&c->state_cond, NULL);
  c->state = CKState_Ready;
  while (recovering)
    sleep(1);
  if (checkpoint_interval <= 0)
      return 0;
  if (ti->index() == 0) {
    for (int i = 1; i < nckthreads; i++)
      while (cks[i].state != CKState_Ready)
        ;
    Str *pv = new Str[nckthreads + 1];
    Json uncommitted_ckp;

    while (1) {
      struct timespec ts;
      set_timespec(ts, now() + (uncommitted_ckp ? 0.25 : checkpoint_interval));

      pthread_mutex_lock(&checkpoint_mu);
      if (!go_quit)
        pthread_cond_timedwait(&checkpoint_cond, &checkpoint_mu, &ts);
      if (go_quit) {
          for (int i = 0; i < nckthreads; i++) {
              cks[i].state = CKState_Quit;
              pthread_cond_signal(&cks[i].state_cond);
          }
          pthread_mutex_unlock(&checkpoint_mu);
          break;
      }
      pthread_mutex_unlock(&checkpoint_mu);

      if (uncommitted_ckp) {
          kvepoch_t mfe = max_flushed_epoch();
          if (!mfe || mfe > uncommitted_ckp["max_epoch"].to_u64()) {
              commit_checkpoint(uncommitted_ckp);
              uncommitted_ckp = Json();
          }
          continue;
      }

      double t0 = now();
      ti->rcu_start();
      for (int i = 0; i < nckthreads + 1; i++)
        pv[i].assign(NULL, 0);
      tree->findpivots(pv, nckthreads + 1);
      ti->rcu_stop();

      kvepoch_t min_epoch = global_log_epoch;
      pthread_mutex_lock(&checkpoint_mu);
      ckp_gen = ckp_gen.next_nonzero();
      for (int i = 0; i < nckthreads; i++) {
          cks[i].startkey = pv[i];
          cks[i].endkey = (i == nckthreads - 1 ? Str() : pv[i + 1]);
          cks[i].state = CKState_Go;
          pthread_cond_signal(&cks[i].state_cond);
      }
      pthread_mutex_unlock(&checkpoint_mu);

      ti->rcu_start();
      conc_filecheckpoint(ti);
      ti->rcu_stop();

      cks[0].state = CKState_Ready;
      uint64_t bytes = cks[0].bytes;
      pthread_mutex_lock(&checkpoint_mu);
      for (int i = 1; i < nckthreads; i++) {
        while (cks[i].state != CKState_Ready)
          pthread_cond_wait(&cks[i].state_cond, &checkpoint_mu);
        bytes += cks[i].bytes;
      }
      pthread_mutex_unlock(&checkpoint_mu);

      uncommitted_ckp = prepare_checkpoint(min_epoch, nckthreads, pv);

      for (int i = 0; i < nckthreads + 1; i++)
        if (pv[i].s)
          free((void *)pv[i].s);
      double t = now() - t0;
      fprintf(stderr, "kvd-ckp-%" PRIu64 " [%s,%s]: prepared (%.2f sec, %" PRIu64 " MB, %" PRIu64 " MB/sec)\n",
              ckp_gen.value(), uncommitted_ckp["min_epoch"].to_s().c_str(),
              uncommitted_ckp["max_epoch"].to_s().c_str(),
              t, bytes / (1 << 20), (uint64_t)(bytes / t) >> 20);
    }
  } else {
    while(1) {
      pthread_mutex_lock(&checkpoint_mu);
      while (c->state != CKState_Go && c->state != CKState_Quit)
        pthread_cond_wait(&c->state_cond, &checkpoint_mu);
      if (c->state == CKState_Quit) {
        pthread_mutex_unlock(&checkpoint_mu);
        break;
      }
      pthread_mutex_unlock(&checkpoint_mu);

      ti->rcu_start();
      conc_filecheckpoint(ti);
      ti->rcu_stop();

      pthread_mutex_lock(&checkpoint_mu);
      c->state = CKState_Ready;
      pthread_cond_signal(&c->state_cond);
      pthread_mutex_unlock(&checkpoint_mu);
    }
  }
  return 0;
}


//Masstree::default_table *tree;
//using lcdf::StringAccum;

//volatile uint64_t globalepoch = 1;     // global epoch, updated by main thread regularly
//volatile uint64_t active_epoch = 1;
//kvtimestamp_t initial_timestamp;
//static pthread_cond_t rec_cond;
//pthread_mutex_t rec_mu;
//static int rec_nactive;
//static int rec_state = REC_NONE;

//volatile bool recovering = false; // so don't add log entries, and free old value immediately

#if SHARED_CQ
struct ibv_cq **sharedCQ;
#endif

#if ENABLE_KERNEL
uint64_t **connArray;
uint64_t sizeofArray = 3008; //125; //3008, 125; //size to 1000 and do loop multiple times
uint64_t numIterations = 1;  //45; // 1, 45
uint64_t numPartitions = 32;
#endif 

float *elapsedTime;
char* output_dir;

float *elapsedBV;


uint64_t gen_arrival_time(uint64_t *arrivalSleepTime) {
    
	static uint16_t index = 0;
	uint64_t result = arrivalSleepTime[index];
	//printf("index = %lu, OR  = %lu \n",index,SERVICE_TIME_SIZE | index);
	if( (SERVICE_TIME_SIZE | index) == 0xFFFF) {
		//printf("worked!, index = %lu \n",index);
		index = 0;
	}
	else {
		//printf("index = %lu \n",index);
		index++;
	}
	return result;
}

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

	struct ibv_mr *mr_write;                  // MR handle for buf
    volatile char *buf_write;                 // memory buffer pointer, used for
};
struct ibv_context	*globalcontext;

struct pingpong_context **ctx;

struct thread_data {
	struct pingpong_context **ctxs;
	uint64_t thread_id;
    threadinfo *ti;
};

void my_sleep(uint64_t n) {
	//if(n == 10000) printf("mysleep = %llu \n",n);
	struct timespec ttime,curtime;
	clock_gettime(CLOCK_MONOTONIC,&ttime);
	
	while(1){
		clock_gettime(CLOCK_MONOTONIC,&curtime);
		uint64_t elapsed = (curtime.tv_sec-ttime.tv_sec )/1e-9 + (curtime.tv_nsec-ttime.tv_nsec);

		if (elapsed >= n) {
			assert(elapsed >= n);
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

	sscanf(msg, "%x:%x:%x:%p:%x:%s", &rem_dest->lid, &rem_dest->qpn,
						&rem_dest->psn, &rem_dest->addr, &rem_dest->rkey, gid);
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
	/*
	if(connfd != 0 )
	{
		char buffer[256];
		strerror_r(errno, buffer, 256 ); // get string message from errno, XSI-compliant version
		printf("Error %s", buffer);
		// or
		char * errorMsg = strerror_r( errno, buffer, 256 ); // GNU-specific version, Linux default
		printf("Error %s", errorMsg); //return value has to be used since buffer might not be modified
		// ...
	}

	if (connfd < 0) {
		if(connfd == EBADF) printf("1 \n");
		else if(connfd == EFAULT) printf("2 \n");
		else if(connfd == EINVAL) printf("3 \n");
		else if(connfd == ENOBUFS) printf("4 \n");
		else if(connfd == EOPNOTSUPP) printf("5 \n");
		else if(connfd == EWOULDBLOCK) printf("6 \n");
		else printf("7 \n");

		printf("connfd = %d \n", connfd);
		fprintf(stderr, "accept() failed\n");
		return NULL;
	}
	*/
	n = read(connfd, msg, sizeof msg);
	if (n != sizeof msg) {
		perror("server read");
		fprintf(stderr, "%d/%d: Couldn't read remote address\n", n, (int) sizeof msg);
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
			//printf("created sharedCQ,  %llu \n", ctx->id);
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
			goto clean_mr;
		}
	#endif

	assert(posix_memalign((void**)(&ctx->buf_write), bufOffset, totalBytes) == 0);
	//ctx->buf_write = (volatile char *)malloc(totalBytes);
	//printf("buf address %p \n",(ctx->buf_write));
 
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
		init_attr.sq_sig_all = 0;

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
	//printf("s7 \n");

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
	list.length = 35;//ctx->size;
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

static int pp_post_send(struct pingpong_context *ctx, bool signal)
{
	struct ibv_sge list;// = {
	memset(&list, 0, sizeof(list));

	list.addr	= (uintptr_t) ctx->buf_send;
	list.length = 30;//ctx->size; //max with RC can be 38 for 3 flits
	list.lkey	= ctx->mr_send->lkey;

	//assert((uint8_t)ctx->buf_send[9] == 0);
	struct ibv_send_wr wr;
	memset(&wr, 0, sizeof(wr));

	wr.wr_id	    = ctx->rx_depth;
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

/*
static int pp_post_write(struct pingpong_context *ctx, bool signal, uint64_t offset)
{
	struct ibv_sge list;// = {
	memset(&list, 0, sizeof(list));

	list.addr	= (uintptr_t) ctx->buf_write + offset;
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

	if(signal == true) wr.send_flags = ctx->send_flags | IBV_SEND_SIGNALED; //need to have a signaled completion every signalInterval
	else wr.send_flags = ctx->send_flags;

	wr.wr.rdma.remote_addr = ctx->rem_dest->addr + offset;
	//printf("remote addr = %x \n", wr.wr.rdma.remote_addr);
	wr.wr.rdma.rkey = ctx->rem_dest->rkey;

	struct ibv_send_wr *bad_wr;
	memset(&bad_wr, 0, sizeof(bad_wr));

	return ibv_post_send(ctx->qp, &wr, &bad_wr);
}
*/

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

	//#if SHARED_CQ
	//uint64_t offset = thread_id;
	//#else
	uint64_t offset = (numConnections/numThreads) * thread_id;
	//#endif
	unsigned int rcnt = 0, scnt = 0;
	uint64_t wrID;
	struct pingpong_context *ctx = ctxs[0];
	//OFFSET IS THE CONNECTION ID

	struct ibv_wc wc[1];
	struct ibv_wc wc_drain[rx_depth*2];

	#if NOTIFICATION_QUEUE
	struct ibv_wc wc_notif[1];
	int ne_notif, i_notif;
	uint64_t wrID_notif;
	struct pingpong_context *ctx_noti = ctxs[1];
	#endif
	
	int ne, i, ne_drain;

	//printf("ctx->id = %llu \n", ctxs[thread_id]->id);
	cpu_set_t cpuset;
    CPU_ZERO(&cpuset);       				   //clears the cpuset
    if(thread_id < 12) CPU_SET(thread_id /*+2*//*12*/, &cpuset);  //set CPU 2 on cpuset
    else CPU_SET(thread_id + 12/*+2*//*12*/, &cpuset);  //set CPU 2 on cpuset
	sched_setaffinity(0, sizeof(cpuset), &cpuset);


    threadinfo* ti = reinterpret_cast<threadinfo*>(tdata->ti);
	ti->pthread() = pthread_self();

    struct kvout *kvout = new_bufkvout();
	msgpack::streaming_parser parser;
	StringAccum sa;
	query<row_type> q;

	/*
	//if(servername) {
		uint64_t singleThreadWait = 1000000000/goalLoad;
		uint64_t avg_inter_arr_ns = numThreads*singleThreadWait;

		//if(thread_num == 6) avg_inter_arr_ns = 10000;

		uint64_t *arrivalSleepTime = (uint64_t *)malloc(SERVICE_TIME_SIZE*sizeof(uint64_t));
		std::random_device rd{};
		std::mt19937 gen{rd()};
		std::exponential_distribution<double> exp{1/((double)avg_inter_arr_ns)};//-SEND_SERVICE_TIME)};
		for(uint64_t i = 0; i < SERVICE_TIME_SIZE; i++) {
			arrivalSleepTime[i] = exp(gen);
			//printf("arrivalSleepTime = %llu \n", arrivalSleepTime[i]);
		}
	//}
	*/

	uint64_t mean = (uint64_t)2E3;
	uint64_t sleep_time = 1000;
	uint64_t foundEmptyQueue = 0;
	uint64_t prevOffset = 0;

	#if FPGA_NOTIFICATION
		/*
		uint32_t *processBufB4Polling = (uint32_t*) malloc(numConnections*sizeof(uint32_t)); 
		for(int q = 0; q < numConnections; q++) {
			//processBufB4Polling[q] = (uint32_t*) malloc(sizeof(uint32_t));
			processBufB4Polling[q] = NULL;
		}
		//uint32_t *processBufB4PollingSrcQP = (uint32_t*) malloc(NUM_QUEUES*sizeof(uint32_t)); 
		//for(int q = 0; q < NUM_QUEUES; q++) {
			//processBufB4PollingSrcQP[q] = (uint32_t*) malloc(sizeof(uint32_t));
		//	processBufB4PollingSrcQP[q] = NULL;
		//}
		*/
		//uint32_t offset = 0;		
		unsigned long long leadingZeros = 0;
		//alignas(64) uint64_t result[8] = {0,0,0,0,0,0,0,0};
		//uint8_t sequenceNumberInBV;
		//bool need2PollAgain = false;

		//__m512i sixtyfour = _mm512_setr_epi64(64, 64, 64, 64, 64, 64, 64, 64);
		//uint64_t log2;
		//uint64_t value;
		//uint64_t byteOffset;
		//uint64_t leadingZerosinValue;
		//uint8_t resultIndexWorkFound;
		//uint64_t bufID;
		//uint64_t received = false;

		//uint64_t startOffset = 4096*thread_id;
		//uint64_t endOffset = startOffset + numConnections;
		//uint64_t leftOff = startOffset;

		uint64_t notificationExistsButNoPacket = 0;
		uint64_t idlePollsBeforeFindingWork = 0;
		//bool haveNotPolled = true;

		//bool currentState = false;

		uint64_t lzcntsaysworkbutactuallynotthere = 0;
		bool foundWork = false;
		uint64_t ii = 0;
		uint64_t counting = 0;
		uint64_t no_work_found_idle_polls = 0;
		//uint64_t alliterations = 0;

		bool exit = false;
		ctx = ctxs[prevOffset];
	#endif

	uint16_t signalInterval = (8192 - ctx->rx_depth)/8;
	printf("signalInterval = %llu \n", signalInterval);

	uint64_t connection8Active = 0;
	uint64_t connection9Active = 0;

	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC,&start);
	
	struct timespec starttimestamp, endtimestamp;	

	struct timespec startBVtimestamp, endBVtimestamp;	
    uint64_t countBVtimes = 0;

    uint64_t threadMismatch = 0;
    bool measure = true;

    #if SHARE_SHAREDCQs
        uint64_t sharedCQIndex = thread_id;
    #endif

    bool warmUpEnded = false;

	while (1) { //exit == false) {

			clock_gettime(CLOCK_MONOTONIC,&end);

			//runtime
            if(warmUpEnded == true) {
                if(end.tv_sec - start.tv_sec > RUNTIME) {
                    break;
                }
            }

			//printf("offset = %llu \n", offset);

			#if !SHARED_CQ && !FPGA_NOTIFICATION && !NOTIFICATION_QUEUE
			if(servername == NULL) {
				ctx = ctxs[offset];
			}
			#endif

			#if FPGA_NOTIFICATION
			/////////////////////////////////Multi Queue - lzcnt //////////////////////////////////////////////
			foundWork = false;

            //clock_gettime(CLOCK_MONOTONIC,&startBVtimestamp);		
            //measure = true;				

			if(ii*8 >= numConnections) { //compare against upperbound for numConnections*8 to make code identical
				ii = 0;
			}
            //ii = ((ii << 3) >= numConnections) ? 0 : ii;
            
			unsigned long long value = htonll(*reinterpret_cast<volatile unsigned long long*>(res.buf + ii + (4096*thread_id))); // << 12)));

			while(value != 0) {

			__asm__ __volatile__ ("lzcnt %1, %0" : "=r" (leadingZeros) : "r" (value):);  //register allocated
			
            value = value & (uint64_t)(0x7FFFFFFFFFFFFFFF >> leadingZeros);
            //value &= ~(1ULL << (63 - leadingZeros));
			
            offset = (ii << 3) + leadingZeros;
            
			foundWork = true;
			ctx = ctxs[offset];

            /*
            if(measure == true) {
                clock_gettime(CLOCK_MONOTONIC,&endBVtimestamp);				
                
                countBVtimes++;
                elapsedBV[thread_id] = elapsedBV[thread_id] + ((endBVtimestamp.tv_sec-startBVtimestamp.tv_sec)*1e9 + (endBVtimestamp.tv_nsec-startBVtimestamp.tv_nsec));	
                measure = false;	
            }
            */
			#endif

			#if SHARED_CQ
				//ne = ibv_poll_cq(sharedCQ[thread_id], connPerThread*2*ctx->rx_depth, wc);
							
				#if SHARE_SHAREDCQs
					ne = ibv_poll_cq(sharedCQ[sharedCQIndex], 1, wc);
					sharedCQIndex++;
					if(sharedCQIndex == numThreads) sharedCQIndex = 0;
				#else
					ne = ibv_poll_cq(sharedCQ[thread_id], 1, wc);
				#endif
				//printf("after poll cq \n");
			#else
				//ne = ibv_poll_cq(ctx->cq, 2*ctx->rx_depth, wc);
				ne = ibv_poll_cq(ctx->cq, 1, wc);
			#endif

			//printf("after polling \n");

			//if(servername) printf("polling \n");

			//my_sleep(uint64_t(3E2)); //620kRPS

			if (ne < 0) {
				fprintf(stderr, "poll CQ failed %d\n", ne);
				return 0;
			}
			#if NOTIFICATION_QUEUE
			if(ne == 0) {
				foundEmptyQueue++;
				while(ne == 0) ne = ibv_poll_cq(ctx->cq, 1, wc);
			}
			#endif

			//#if FPGA_NOTIFICATION
			//if(foundWork == true && ne == 0) lzcntsaysworkbutactuallynotthere++;
			//#endif

			for (i = 0; i < ne; ++i) {
				//printf("hello \n");
                kvout_reset(kvout);
                //printf("Buffer length after = %d \n", buf.length());
                parser.reset();
                //if(debug) printf("parse_state 1 = %d \n", parser.state_);

				if (wc[i].status != IBV_WC_SUCCESS) {
					fprintf(stderr, "Failed status %s (%d) for wr_id %d\n", ibv_wc_status_str(wc[i].status),wc[i].status, (int) wc[i].wr_id);
					return 0;
				}
				wrID = (uint64_t) wc[i].wr_id;

				#if SHARED_CQ
					uint64_t offset = wc[i].qp_num & (MAX_QUEUES-1);
					//printf("offset = %llu , qpn = %llu \n", offset, wc[i].qp_num);
					ctx = ctxs[offset];
				#endif

				#if CONN_AFFINITY_OVERHEAD
					if(prevOffset != offset) my_sleep(1E2);
					prevOffset = offset;
				#endif

				if(wrID == ctx->rx_depth) {
					//++ctx->scnt;
					//scnt++;
					//printf("send complete ... scnt = %llu \n", ctx->scnt);
					//my_sleep(uint64_t(3E2)); //75RPS
				}
				else if (wrID >= 0 && wrID < ctx->rx_depth) {
					//printf("received pkt \n");
                    /*
                    for(int p = 0; p < 40; p++)
					{	
						printf("%x",(ctx->buf_recv[wrID][p]));
					}
					printf("\n");
                    */
                    /*
                    uint64_t connID = wc[i].qp_num & (MAX_QUEUES-1);
                    switch(connID)
                    {
                        case 0 ... 84:
                        {
                            assert(thread_id == 0);
                            break;
                        }
                        case 85 ... 169:
                        {
                            assert(thread_id == 1);
                            break;
                        }
                        case 170 ... 254:
                        {
                            assert(thread_id == 2);
                            break;
                        }					
                        case 255 ... 339:
                        {
                            assert(thread_id == 3);
                            break;
                        }					
                        case 340 ... 424:
                        {
                            assert(thread_id == 4);
                            break;
                        }					
                        case 425 ... 509:
                        {
                            assert(thread_id == 5);
                            break;
                        }					
                        case 510 ... 594:
                        {
                            assert(thread_id == 6);
                            break;
                        }					
                        case 595 ... 679:
                        {
                            assert(thread_id == 7);
                            break;
                        }					
                        case 680 ... 764:
                        {
                            assert(thread_id == 8);
                            break;
                        }					
                        case 765 ... 849:
                        {
                            assert(thread_id == 9);
                            break;
                        }					
                        case 850 ... 934:
                        {
                            assert(thread_id == 10);
                            break;
                        }					
                        case 935 ... 1019:
                        {
                            assert(thread_id == 11);
                            break;
                        }		
                        default:
                        {
                        }			
                    }
                    */

                    /*
                    #if !FPGA_NOTIFICATION
                        ctx->buf_recv[wrID][9] = (uint8_t)thread_id;
                    #else 
                        assert((uint8_t)ctx->buf_recv[wrID][9] < numThreads);
                        if((uint8_t)ctx->buf_recv[wrID][9] != (uint8_t)thread_id) threadMismatch++; //printf("mismatch ... FPGA said thread %llu, thread id = %llu \n", (uint8_t)ctx->buf_recv[wrID][9], (uint8_t)thread_id);
                        ctx->buf_recv[wrID][9] = (((uint8_t)thread_id << 4) & 0xF0) + (uint8_t)ctx->buf_recv[wrID][9] & 0x0F;          
                    #endif
                    */

					clock_gettime(CLOCK_MONOTONIC,&starttimestamp);						
					//my_sleep(uint64_t(1E3));
					
					
                    unsigned consumed = parser.consume(ctx->buf_recv[wrID]+10, 256, String(ctx->buf_recv[wrID]+10));
                    //printf("consumed = %llu \n", consumed);
                    if (parser.success() && parser.result().is_a()) 
					{
						//ti->rcu_start();
                        //if(debug) printf("parse_state 4 = %d \n", parser.state_);

						if (onego(q, parser.result(), Str(ctx->buf_recv[wrID]+10, consumed), *ti) >= 0) 
						{
                            //printf("in onego, sa.length =  %llu \n", consumed);
							for(int p = 0; p < sa.length(); p++)
							{	
								sa.data()[p] = 0x00;
							}
							sa.clear();
                            
							msgpack::unparser<StringAccum> cu(sa);
							cu << parser.result();
						}
						else printf("onego failed 0 \n");
						//ti->rcu_stop();
					} 
					else {
						printf("parser failed 1 \n");
						return NULL;
					}
							
					clock_gettime(CLOCK_MONOTONIC,&endtimestamp);

                    ctx->buf_recv[wrID][9] = (((uint8_t)thread_id << 4) & 0xF0) + ((uint8_t)ctx->buf_recv[wrID][9] & 0x0F);          
					memcpy(ctx->buf_send,ctx->buf_recv[wrID],10);
					//memcpy(ctx->buf_send+10,sa.data(), 20); //sa.length());

					//memset(ctx->buf_recv[wrID], 0x00, size);


					if(servername == NULL) {
						/*
						if(offset != (uint8_t)ctx->buf_recv[wrID][19]) {
							printf("offset = %lu , priority = %lu \n", offset, (uint8_t)ctx->buf_recv[wrID][19]);
							assert(offset == (uint8_t)ctx->buf_recv[wrID][19]);
						}
						*/

						//if ((uint8_t)ctx->buf_recv[wrID][0] >= 128) foundEmptyQueue++; //printf("T%llu - conn %llu is empty \n", thread_id, offset);

						if(((uint8_t)ctx->buf_recv[wrID][0] >> 4) & 0x01 == 1) {
							//printf("hello! ");

                            if(warmUpEnded == false) {
                                warmUpEnded = true;
                                clock_gettime(CLOCK_MONOTONIC,&start);
                            }

							rcnt++;

							#if SHARED_CQ
								countPriority[thread_id][offset]++;
							#else
								countPriority[thread_id][offset]++;
							#endif

							//printf("recv complete ... rcnt = %llu \n", ctx->rcnt);
							
							//elapsedTime[thread_id] = elapsedTime[thread_id] + (endTS - startTS);
							elapsedTime[thread_id] = elapsedTime[thread_id] + ((endtimestamp.tv_sec-starttimestamp.tv_sec)*1e9 + (endtimestamp.tv_nsec-starttimestamp.tv_nsec));

						}

					}

					assert(pp_post_recv(ctx, wrID) == 0);
					if (ctx->routs < ctx->rx_depth) {
						fprintf(stderr,
							"Couldn't post receive (%d)\n", ctx->routs);
						return 0;
					}

					//printf("offset = %llu , qpn = %llu \n", offset, wc[i].qp_num);

						
						uint64_t success = pp_post_send(ctx, (ctx->scnt%signalInterval == 0));
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
                        //assert(success == 0);

						scnt++;
						ctx->scnt++;
				}
				else {
					fprintf(stderr, "Completion for unknown wr_id %d\n",(int) wc[i].wr_id);
					return 0;
				}
			}

			#if !SHARED_CQ && !FPGA_NOTIFICATION && !NOTIFICATION_QUEUE		
			if(servername == NULL) {
				if(offset == numConnections-1) offset = 0;
				else offset++;

				//if(offset == ((numConnections/numThreads) * (thread_id+1)) -1) offset = (numConnections/numThreads) * thread_id;
				//else offset++;
				
			}
			#endif
					
			#if FPGA_NOTIFICATION
			//printf("hi \n");
			}
			ii+=8;
			//if(ii >= numConnections) ii = 0;
			
			#endif
	}
	
	//printf("T %d, %d iters, elapsedTime = %f \n", thread_id, rcnt, elapsedTime[thread_id]);

	elapsedTime[thread_id] = elapsedTime[thread_id]/rcnt;
    //printf("hi \n");
    //elapsedBV[thread_id] = elapsedBV[thread_id]/countBVtimes;
	ctx = ctxs[offset];

	float nsec = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
	//long long bytes = (long long) size * iters * 2;
		
	all_rcnts[thread_id] = rcnt;
	all_scnts[thread_id] = scnt;

	rps[thread_id] = rcnt/(nsec/1000000000.);
	#if FPGA_NOTIFICATION
	//printf("T %d, %d iters in %.5f seconds, rps = %f ..... notificationExistsButNoPacket = %llu , idlePollsBeforeFindingWork = %llu \n", thread_id, rcnt, nsec/1000000000., rps[thread_id],notificationExistsButNoPacket, idlePollsBeforeFindingWork);
	//#else
    //printf("T%d - countBVtimes = %llu \n", thread_id, countBVtimes);
    //printf("T%d - threadMismatch = %llu \n", thread_id, threadMismatch);
    #endif
	printf("T %d, %d iters in %.5f seconds, rps = %f  \n", thread_id, rcnt, nsec/1000000000., rps[thread_id]);
	
	//printf("T%llu found an empty queue %llu times \n", thread_id, foundEmptyQueue);
	//printf("connection 8 active = %llu \n", connection8Active);
	//printf("connection 9 active = %llu \n", connection9Active);

	/*
	//printf("\n\nprinting sequence number stats \n.\n.\n.\n");
	uint64_t totalOutOfOrderNum = 0;
	for(uint64_t h = 0; h < numConnections; h++) {
		//printf("%llu  ", outOfOrderNum[h]);
		totalOutOfOrderNum += outOfOrderNum[h];
	}
	printf("\nT%d totalOutOfOrderNum = %llu \n", thread_id, totalOutOfOrderNum);
	*/

	//printf(".\n.\n.\nend of sequence number stats ... \n");

	//printf("%lld bytes in %.2f seconds = %.2f Mbit/sec\n", bytes, usec / 1000000., bytes * 8. / usec);
	//printf("%d iters in %.2f seconds = %.2f usec/iter\n", iters, usec / 1000000., usec / iters);

	//free(ctx->rem_dest);

	//end of thread operations

	#if FPGA_NOTIFICATION
	//printf("counting = %llu \n", counting);
	//printf("lzcntsaysworkbutactuallynotthere = %llu \n", lzcntsaysworkbutactuallynotthere);
	//printf("no_work_found_idle_polls = %llu \n", no_work_found_idle_polls);
	//printf("alliterations = %llu \n", alliterations);
	#endif

	return 0;
}

int main(int argc, char *argv[])
{
	while (1) {
		int c;

		static struct option long_options[] = {
			{ .name = "port",     .has_arg = 1, .flag = NULL, .val = 'p' },
			{ .name = "ib-dev",   .has_arg = 1, .flag = NULL, .val = 'd' },
			{ .name = "ib-port",  .has_arg = 1, .flag = NULL, .val = 'i' },
			{ .name = "size",     .has_arg = 1, .flag = NULL, .val = 's' },
			{ .name = "mtu",      .has_arg = 1, .flag = NULL, .val = 'm' },
			{ .name = "rx-depth", .has_arg = 1, .flag = NULL, .val = 'r' },
			{ .name = "iters",    .has_arg = 1, .flag = NULL, .val = 'n' },
			{ .name = "load",     .has_arg = 1, .flag = NULL, .val = 'l' },
			{ .name = "events",   .has_arg = 0, .flag = NULL, .val = 'e' },
			{ .name = "gid-idx",  .has_arg = 1, .flag = NULL, .val = 'g' },
			{ .name = "numthreads", .has_arg = 1, .flag = NULL, .val = 't' },
			{}
		};

		c = getopt_long(argc, argv, "p:d:i:s:m:r:n:l:eg:c:t:k:f:",
				long_options, NULL);
		if (c == -1)
			break;

		switch (c) {
		case 'p':
			port = strtoul(optarg, NULL, 0);
			if (port > 65535) {
				usage(argv[0]);
				return 1;
			}
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


    threadinfo *main_ti = threadinfo::make(threadinfo::TI_MAIN, -1);
    main_ti->pthread() = pthread_self();

    initial_timestamp = timestamp();
    tree = new Masstree::default_table;
    tree->initialize(*main_ti);
    initial_timestamp = timestamp();
	
	//600*1024*8 = 4.6 MB
	//2400*1024*8 = 18.75 MB
	//3008*1024*8 = 18.75 MB

	//std::random_device rd1{};
	//std::mt19937 gen1{rd1()};
	//std::uniform_int_distribution<uint8_t>* rand1 = new std::uniform_int_distribution<uint8_t>(0, 255);

	#if ENABLE_KERNEL
	printf("size of conn context = %llu \n", sizeof(struct pingpong_context));
	connArray = (uint64_t**)malloc(numConnections*sizeof(uint64_t*));

	for(uint64_t cc = 0; cc < numConnections; cc++){
		connArray[cc] = (uint64_t*)malloc(sizeofArray*sizeof(uint64_t));
		for(uint64_t ccc = 0; ccc < sizeofArray; ccc++){
			connArray[cc][ccc] = 10;
		}
	}
	#endif

	elapsedTime = (float*)malloc(numThreads*sizeof(float));
	for(int g = 0; g < numThreads; g++) elapsedTime[g] = 0;

	elapsedBV = (float*)malloc(numThreads*sizeof(float));
	for(int g = 0; g < numThreads; g++) elapsedBV[g] = 0;

	if (optind == argc - 1)
		servername = strdupa(argv[optind]);
	else if (optind < argc) {
		usage(argv[0]);
		return 1;
	}

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
		//memset(ctx[y], 0, sizeof(struct pingpong_context*));
		//printf("y = %llu \n", y);
		ctx[y]  = pp_init_ctx(ib_dev, size, rx_depth, ib_port);
		if (!ctx[y])
			return 1;

		ctx[y]->id = y;
		ctx[y]->routs = 0;
		//ctx[y]->souts = 0;
		ctx[y]->scnt = 0;
		ctx[y]->rcnt = 0;
		for(uint64_t z = 0; z < ctx[y]->rx_depth; z++) {
			assert(pp_post_recv(ctx[y], z) == 0);
			ctx[y]->routs++;
		}

		if (ctx[y]->routs < ctx[y]->rx_depth) {
			fprintf(stderr, "Couldn't post receive (%d)\n", ctx[y]->routs);
			return 1;
		}
		
		if (pp_get_port_info(ctx[y]->context, ib_port, &ctx[y]->portinfo)) {
			fprintf(stderr, "Couldn't get port info\n");
			return 1;
		}

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

		if(y  == 0) printf("ready to connect! \n");
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

	do_uc(ib_devname, servername, port, ib_port, gidx, numConnections, numThreads);
	//printf("after UC \n");
	pthread_t pt[numConnections];
    int ret;

	//numThreads = 1;//numConnections;

    rps = (double*)malloc((numThreads)*sizeof(double));
	all_rcnts = (uint32_t*)malloc(numThreads*sizeof(uint32_t));
	all_scnts = (uint32_t*)malloc(numThreads*sizeof(uint32_t));
	countPriority = (uint64_t **)malloc(numThreads*sizeof(uint64_t *));

	expectedSeqNum = (uint8_t *)malloc(numConnections*sizeof(uint8_t));
	for(int c = 0; c < numConnections; c++) expectedSeqNum[c] = 0;

	for(uint16_t t = 0; t < numThreads; t++) {
		countPriority[t] = (uint64_t *)malloc(numConnections*sizeof(uint64_t));
		for(uint16_t g = 0; g < numConnections; g++) countPriority[t][g] = 0;
	}

	//if(servername == NULL) numThreads = 1;
    for(uint16_t x = 0; x < numThreads; x++) {
		struct thread_data *tdata = (struct thread_data *)malloc(sizeof(struct thread_data));

		tdata->ctxs = ctx;
		tdata->thread_id = x;

        threadinfo *ti = threadinfo::make(threadinfo::TI_PROCESS, x);
	    tdata->ti = ti;

		ret = pthread_create(&pt[x], NULL, threadfunc, tdata); 
		assert(ret == 0);
	}

    for(uint16_t x = 0; x < numThreads; x++) {
        int ret = pthread_join(pt[x], NULL);
        assert(!ret);
    }

	//#if ENABLE_KERNEL
	float avgKernelServiceTime = 0.0;
	float avgBVServiceTime = 0.0;
	for(int g = 0; g < numThreads; g++) {
		//printf("elapsedTime = %llu\n", elapsedTime[g]);
		avgKernelServiceTime += elapsedTime[g];
        avgBVServiceTime += elapsedBV[g];
	}
	printf("avgKernelServiceTime = %f\n", avgKernelServiceTime/numThreads);
	//printf("avgBVServiceTime = %f\n", avgBVServiceTime/numThreads);

	char* output_name_avgServiceTime;
	asprintf(&output_name_avgServiceTime, "%s/avg_service_time.servtime", output_dir);
	FILE *f_avgServiceTime = fopen(output_name_avgServiceTime, "wb");
	fprintf(f_avgServiceTime, "%f \n", avgKernelServiceTime/numThreads);
	//fprintf(f_avgServiceTime, "%f \n", avgKernelServiceTime);
	fclose(f_avgServiceTime);

	//sleep(10);
	//#endif

	uint32_t total_rcnt = 0;
	uint32_t total_scnt = 0;

	for(uint16_t i = 0; i < numThreads; i++){
		//printf("allrcnt[%d] = %llu \n", i, all_rcnts[i]);
		total_rcnt += all_rcnts[i];
		total_scnt += all_scnts[i];
	}


	uint64_t *totalPerThread = (uint64_t *)malloc(numThreads*sizeof(uint64_t));
	for(uint16_t t = 0; t < numThreads; t++) totalPerThread[t] = 0;

	for(uint16_t g = 0; g < numConnections; g++) {
		printf("P%llu   ",g);

		for(uint16_t t = 0; t < numThreads; t++){
			totalPerThread[t] += countPriority[t][g];
			printf("%llu    ",countPriority[t][g]);
		}
		printf("\n");
	}		
	printf("\n");
	printf("total per thread ... \n");
	for(uint16_t t = 0; t < numThreads; t++){
		printf("%llu   ",totalPerThread[t]);
	}
	printf("\n");
	printf("total rcnt = %d, total scnt = %d \n",(int)total_rcnt,(int)total_scnt);


	for(uint16_t i = 0; i < numThreads; i++){
		printf("rps = %f \n",rps[i]);
		totalRPS += rps[i];
	}
	printf("%f \n",totalRPS);
	//printf("closing connections \n");

	for(uint64_t z = 0; z < numConnections; z++) {
	if (pp_close_ctx(ctx[z]))
		return 1;
	}
	//printf("closed connections \n");

	ibv_free_device_list(dev_list);
	//printf("free device list \n");

	return 0;
}