# pthread-2PL-project

using mutex to implement 2PL
===

pthread code example:
==
#include <stdio.h><br>
#include <pthread.h><br>
#include <unistd.h><br>

// 子執行緒函數<br>
void* child(void* data){<br>
  char *str = (char*) data; // 取得輸入資料<br>
  for(int i = 0;i < 3;++i) {<br>
    printf("%s\n", str); // 每秒輸出文字<br>
    sleep(1);<br>
  }<br>
  pthread_exit(NULL); // 離開子執行緒<br>
}<br>

// 主程式<br>
int main() {<br>
  pthread_t t; // 宣告 pthread 變數<br>
  pthread_create(&t, NULL, child, "Child"); // 建立子執行緒<br>

  // 主執行緒工作<br>
  for(int i = 0;i < 3;++i) {<br>
    printf("Master\n"); // 每秒輸出文字<br>
    sleep(1);<br>
  }<br>

  pthread_join(t, NULL); // 等待子執行緒執行完成<br>
  return 0;<br>
}<br>


mutex code example
==
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// 計數器
int counter = 0;

// 子執行緒函數
void* child() {
  for(int i = 0;i < 3;++i) {
    int tmp = counter;
    sleep(1); // 故意讓它延遲一下
    counter = tmp + 1;
    printf("Counter = %d\n", counter);
  }
  pthread_exit(NULL);
}

// 主程式
int main() {<br>
  pthread_t t1, t2;<br>
  pthread_create(&t1, NULL, child, NULL);<br>
  pthread_create(&t2, NULL, child, NULL);<br>
  pthread_join(t1, NULL);<br>
  pthread_join(t2, NULL);<br>
  return 0;<br>
}<br>

the answer will be:<br>
Counter = 1<br>
Counter = 1<br>
Counter = 2<br>
Counter = 2<br>
Counter = 3<br>
Counter = 3<br>

if we use mutex
=
#include <stdio.h><br>
#include <pthread.h><br>
#include <unistd.h><br>

// 計數器<br>
int counter = 0;<br>

// 加入 Mutex<br>
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;<br>

// 子執行緒函數<br>
void* child() {<br>
  for(int i = 0;i < 3;++i) {<br>
    pthread_mutex_lock( &mutex1 ); // 上鎖<br>
    int tmp = counter;<br>
    sleep(1);<br>
    counter = tmp + 1;<br>
    pthread_mutex_unlock( &mutex1 ); // 解鎖<br>
    printf("Counter = %d\n", counter);<br>
  }<br>
  pthread_exit(NULL);<br>
}<br>

// 主程式<br>
int main() {<br>
  pthread_t t1, t2;<br>
  pthread_create(&t1, NULL, child, NULL);<br>
  pthread_create(&t2, NULL, child, NULL);<br>
  pthread_join(t1, NULL);<br>
  pthread_join(t2, NULL);<br>
  return 0;<br>
}<br>

the answer will be:<br>
Counter = 1<br>
Counter = 2<br>
Counter = 3<br>
Counter = 4<br>
Counter = 5<br>
Counter = 6<br>

Semaphore 
==
#include <stdio.h> <br>
#include <pthread.h><br>
#include <unistd.h><br>
#include <semaphore.h><br>

sem_t semaphore; // 旗標<br>
int counter = 0;<br>

// 子執行緒函數<br>
void* child() {<br>
  for(int i = 0;i < 5;++i) {<br>
    sem_wait(&semaphore); // 等待工作<br>
    printf("Counter = %d\n", ++counter);<br>
    sleep(1);<br>
  }<br>
  pthread_exit(NULL);<br>
}<br>

// 主程式<br>
int main(void) {<br>

  // 初始化旗標，僅用於本行程，初始值為 0<br>
  sem_init(&semaphore, 0, 0);<br>

  pthread_t t;<br>
  pthread_create(&t, NULL, child, NULL);<br>

  // 送出兩個工作<br>
  printf("Post 2 jobs.\n");<br>
  sem_post(&semaphore);<br>
  sem_post(&semaphore);<br>
  sleep(4);<br>

  // 送出三個工作<br>
  printf("Post 3 jobs.\n");<br>
  sem_post(&semaphore);<br>
  sem_post(&semaphore);<br>
  sem_post(&semaphore);<br>

  pthread_join(t, NULL);<br>

  return 0;<br>
}<br>

[reference](https://blog.gtwang.org/programming/pthread-multithreading-programming-in-c-tutorial/)
