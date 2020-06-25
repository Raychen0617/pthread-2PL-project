# pthread-2PL-project

using mutex to implement 2PL to add a file with 100 data<br>
notice that you can only run the file on `linux`

## pthread code example:
```
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// 子執行緒函數
void* child(void* data){
  char *str = (char*) data; // 取得輸入資料
  for(int i = 0;i < 3;++i) {
    printf("%s\n", str); // 每秒輸出文字
    sleep(1);
  }
  pthread_exit(NULL); // 離開子執行緒
}

// 主程式
int main() {
  pthread_t t; // 宣告 pthread 變數
  pthread_create(&t, NULL, child, "Child"); // 建立子執行緒

  // 主執行緒工作
  for(int i = 0;i < 3;++i) {
    printf("Master\n"); // 每秒輸出文字
    sleep(1);
  }

  pthread_join(t, NULL); // 等待子執行緒執行完成
  return 0;
}
```

## mutex code example:

```
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
int main() {
  pthread_t t1, t2;
  pthread_create(&t1, NULL, child, NULL);
  pthread_create(&t2, NULL, child, NULL);
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  return 0;
}
```

>the answer will be:<br>
Counter = 1<br>
Counter = 1<br>
Counter = 2<br>
Counter = 2<br>
Counter = 3<br>
Counter = 3<br>

### to solve the problem, we use mutex

```
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// 計數器
int counter = 0;

// 加入 Mutex
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

// 子執行緒函數
void* child() {
  for(int i = 0;i < 3;++i) {
    pthread_mutex_lock( &mutex1 ); // 上鎖
    int tmp = counter;
    sleep(1);
    counter = tmp + 1;
    pthread_mutex_unlock( &mutex1 ); // 解鎖
    printf("Counter = %d\n", counter);
  }
  pthread_exit(NULL);
}

// 主程式
int main() {
  pthread_t t1, t2;
  pthread_create(&t1, NULL, child, NULL);
  pthread_create(&t2, NULL, child, NULL);
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  return 0;
}
```
>the answer will be:<br>
Counter = 1<br>
Counter = 2<br>
Counter = 3<br>
Counter = 4<br>
Counter = 5<br>
Counter = 6<br>

## Semaphore code example:
```
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

sem_t semaphore; // 旗標
int counter = 0;

// 子執行緒函數
void* child() {
  for(int i = 0;i < 5;++i) {
    sem_wait(&semaphore); // 等待工作
    printf("Counter = %d\n", ++counter);
    sleep(1);
  }
  pthread_exit(NULL);
}

// 主程式
int main(void) {

  // 初始化旗標，僅用於本行程，初始值為 0
  sem_init(&semaphore, 0, 0);

  pthread_t t;
  pthread_create(&t, NULL, child, NULL);

  // 送出兩個工作
  printf("Post 2 jobs.\n");
  sem_post(&semaphore);
  sem_post(&semaphore);
  sleep(4);

  // 送出三個工作
  printf("Post 3 jobs.\n");
  sem_post(&semaphore);
  sem_post(&semaphore);
  sem_post(&semaphore);

  pthread_join(t, NULL);

  return 0;
}
```
>Post 2 jobs.<br>
Counter = 1<br>
Counter = 2<br>
Post 3 jobs.<br>
Counter = 3<br>
Counter = 4<br>
Counter = 5<br>

# more info:
[reference](https://blog.gtwang.org/programming/pthread-multithreading-programming-in-c-tutorial/)
