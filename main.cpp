#include <bits/stdc++.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <chrono>
using namespace std;
int read_time[70]={0}, ray[70]={0};
int c=0;


void* print(void*);
int read(FILE* );
sem_t semaphore, semaphore_back, lock_finish[100], finish;
pthread_mutex_t job[105];
queue<string> job_list;
int v[105]={0};

FILE *fp, *file;


typedef struct thdata_ {
    string str=" ";
    int number;
}thdata;

void* print(void* ptr) {                // pthread function
    while(true){
        
        sem_wait(&semaphore);
        thdata* data;
        data = (thdata*) ptr;
        data->str = job_list.front();
        data->number = c;
        c++;
        job_list.pop();
        sem_post(&semaphore_back);
        //cout<<"queueing"<<endl;

        vector<int> minus_var;
        vector<int> plus_var;

        int read_start;
        for(int i=0;i<data->str.size();i++){
            if(data->str[i]=='='){
                read_start = i;
                break;
            }
        }

        char tag;
        int answer=0;
        
        string s;
        for(int i=read_start+3;i<data->str.size();i++){

            //cout<<"str[i] = "<<data->str[i]<<endl;
            if(data->str[i]-'0'>=0 && data->str[i]-'0'<10){
                s+=data->str[i];  
            }
            else
            {
                plus_var.push_back(stoi(s));
                break;
            }
        }
        for(int i=read_start+4;i<data->str.size();i++){
            if(data->str[i]==' '){
                continue;
            }
            else if(data->str[i]=='-'){
                i+=2;
                if(data->str[i]!='$'){
                    string minus;
                    while(data->str[i]-'0'>=0 && data->str[i]-'0'<10){
                        minus+=data->str[i];
                        i++;
                    }
                    answer -= stoi(minus);
                    //cout<<"minus "<<minus<<endl;
                }
                else{             
                    i++;
                    string minus;
                    while(data->str[i]-'0'>=0 && data->str[i]-'0'<10){
                        minus+=data->str[i];
                        i++;
                    }          
                    minus_var.push_back(stoi(minus));
                }
            }
            else if(data->str[i]=='+'){
                i+=2;
                if(data->str[i]!='$'){
                    string plus;
                    while(data->str[i]-'0'>=0 && data->str[i]-'0'<10){
                        plus+=data->str[i];
                        i++;
                    }
                    answer += stoi(plus);
                    //cout<<"plus"<<plus<<endl;
                }
                else{
                    i++;
                    string plus;
                    while(data->str[i]-'0'>=0 && data->str[i]-'0'<10){
                        plus+=data->str[i];
                        i++;
                    }
                    plus_var.push_back(stoi(plus));   
                }   
            }
        }

        int var;
        string s1;
        for(int i=0;i<data->str.size();i++){
            if(data->str[i]=='$'){
                i++;
                while(data->str[i]-'0'>=0 && data->str[i]-'0'<10){
                    s1+=data->str[i];
                    i++;
                }
                var = stoi(s1);
                break;
            }
        } 
        sem_wait(&lock_finish[data->number]);
        pthread_mutex_lock (&job[var]);
        map <int,int> MAP;
        
        for(int i=0;i<minus_var.size();i++){

            if(read_time[minus_var[i]]==0 && MAP[minus_var[i]]==0 && minus_var[i]!=var){
                pthread_mutex_lock (&job[minus_var[i]]);
                //read_time[minus_var[i]]++;
                MAP[minus_var[i]]++;
            }
            ray[minus_var[i]]++;         
            //cout<<"minus ＄"<<minus_var[i]<<"= "<<v[minus_var[i]]<<endl;
            
        }
        
        for(int i=0;i<plus_var.size();i++){
            
            if(read_time[plus_var[i]]==0 && MAP[plus_var[i]]==0 && plus_var[i]!=var){
                pthread_mutex_lock (&job[plus_var[i]]);
                //read_time[plus_var[i]]++;
                MAP[plus_var[i]]++;
            }
            ray[plus_var[i]]++;
            //cout<<"plus ＄"<<plus_var[i]<<"= "<<v[plus_var[i]]<<endl;
            
        }
        
        sem_post(&lock_finish[data->number+1]);

        for(int i=0;i<plus_var.size();i++){
            answer += v[plus_var[i]];
            ray[plus_var[i]]--;

            if(ray[plus_var[i]]==0 && read_time[plus_var[i]]==0 && MAP[plus_var[i]]==1 && plus_var[i]!=var){
                //read_time[plus_var[i]]--;
                pthread_mutex_unlock (&job[plus_var[i]]);
                MAP[plus_var[i]]--;
            }
            
        }
        
        for(int i=0;i<minus_var.size();i++){  
            answer -= v[minus_var[i]];
            ray[minus_var[i]]--;
            if(ray[minus_var[i]]==0 && read_time[minus_var[i]]==0 && MAP[minus_var[i]]==1 && minus_var[i]!=var){
                //read_time[minus_var[i]]--;
                pthread_mutex_unlock (&job[minus_var[i]]);
                MAP[minus_var[i]]--;
            }
            
        }

        v[var]=answer;
        pthread_mutex_unlock (&job[var]); 
        //fprintf(file,"post\n");
        sem_post(&finish);
        
    }
}
int n;
void readfile(){
    string s1;
    cin>>n;
    for(int i=0;i<n;i++){
        cin>>v[i];
    }
    getchar();  
    while(getline(cin,s1)){
        job_list.push(s1);
    }
}

int main(int argc, char *argv[]){
    
    auto start = chrono::system_clock::now();
    
    char ch;
    int num_threads = stoi(argv[1]);
    file=fopen(argv[2],"w");
    readfile();
    
    
    for (int i=0; i<num_threads; i++) { 
        pthread_t* thread = new pthread_t;
        thdata* data = new thdata;
        pthread_create(thread, NULL, &print, data);
    }
    
    //fprintf(file,"post\n");
    sem_post(&lock_finish[0]);

    while(!job_list.empty()){
        sem_post(&semaphore);
        sem_wait(&semaphore_back);
        //fprintf(file,"post\n");
    }
    
    for(int i=0;i<101;i++){
        sem_wait(&finish);
        //fprintf(file,"wait\n");
    }

    for(int i=0;i<n;i++){
        //cout<<"$"<<i<<" = "<<v[i]<<endl;
        fprintf(file,"$%d = %d\n",i,v[i]);
    }
    cout << "Time: " << chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - start).count() << endl;
    return 0;
}