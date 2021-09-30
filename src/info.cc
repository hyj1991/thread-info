#include "nan.h"
#include <mach/mach_init.h>
#include <mach/thread_act.h>
#include <mach/mach_port.h>

namespace threadinfo
{

  void cpu_usage(pid_t pid)
  {
    task_t port;
    task_for_pid(mach_task_self(), pid, &port);

    task_info_data_t tinfo;
    kern_return_t kr;
    mach_msg_type_number_t task_info_count;

    task_info_count = TASK_INFO_MAX;
    kr = task_info(port, TASK_BASIC_INFO, (task_info_t)tinfo, &task_info_count);
    printf("\n\n<%d> task_threads: %d\n", pid, kr);
    if (kr != KERN_SUCCESS)
      return;

    task_basic_info_t basic_info;
    thread_array_t thread_list;
    mach_msg_type_number_t thread_count;

    thread_info_data_t thinfo;
    mach_msg_type_number_t thread_info_count;

    thread_basic_info_t basic_info_th;
    uint32_t stat_thread = 0; // Mach threads

    basic_info = (task_basic_info_t)tinfo;

    // get threads in the task
    kr = task_threads(port, &thread_list, &thread_count);

    if (kr != KERN_SUCCESS)
      return;
    if (thread_count > 0)
      stat_thread += thread_count;

    long tot_sec = 0;
    long tot_usec = 0;
    long tot_cpu = 0;
    int j;

    for (j = 0; j < thread_count; j++)
    {
      thread_info_count = THREAD_INFO_MAX;
      kr = thread_info(thread_list[j], THREAD_BASIC_INFO,
                       (thread_info_t)thinfo, &thread_info_count);
      if (kr != KERN_SUCCESS)
        continue;
      basic_info_th = (thread_basic_info_t)thinfo;

      if (!(basic_info_th->flags & TH_FLAGS_IDLE))
      {
        tot_sec = tot_sec + basic_info_th->user_time.seconds + basic_info_th->system_time.seconds;
        tot_usec = tot_usec + basic_info_th->system_time.microseconds + basic_info_th->system_time.microseconds;
        tot_cpu = tot_cpu + basic_info_th->cpu_usage;
      }

      printf("[thread %d] tot_sec: %ld, tot_usec: %ld, tot_cpu: %ld\n\n", j + 1, tot_sec, tot_usec, basic_info_th->cpu_usage);
    }
  }

  NAN_METHOD(showThreadUsage)
  {
    pid_t pid = Nan::To<uint32_t>(info[0]).ToChecked();
    cpu_usage(pid);
  }

  void Initialize(v8::Local<v8::Object> target)
  {
    Nan::HandleScope scope;
    Nan::Set(target, Nan::New<v8::String>("name").ToLocalChecked(),
             Nan::New<v8::String>("thread-info").ToLocalChecked());
    Nan::SetMethod(target, "showThreadUsage", showThreadUsage);
  }

  NODE_MODULE(info, Initialize)
}
