
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <bitset>

//using namespace std;
/*
#include <cstudio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <bitsream>
*/





unsigned int bintohex(char *digits){
  unsigned int res=0;
  while(*digits)
    res = (res<<1)|(*digits++ -'0');
  return res;
}


bool char_equal(char* a, char* b, int N){
  int count=0;
  for(int i=0;i<N;++i)
  {
    if(a[i]==b[i])count++;
  }
  return count==N;
}

int get_setindex(char* addrBin, int tag_size, int index_size){
  int index_value=0;
 // printf("%s\n",addrBin);
  for(int i=0;i<index_size;++i){
    if(addrBin[tag_size+i]=='1')
    {
    index_value+= pow(2,index_size-i-1);
    }
  }
 // printf("%d\n", index_value);
  return index_value;
}

/*

struct cache_status
{
  int access_count;  
  int read_count; 
  int write_count; 
  int replacement_count;
  int miss_count;
  int hit_count;
  int read_miss_count;
  int write_miss_count; 
  int read_hit_count;
  int write_hit_count;
  int write_back_count; 
  double hit_rate;
  double miss_rate;
  int total_memory_traffic;
} status ={0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void printstatus(int write_policy)
{
  
  status.hit_count=status.read_hit_count + status.write_hit_count;
  status.hit_rate=(double) status.hit_count/status.access_count;
  status.miss_rate=(double) status.miss_count/status.access_count;
  if(!write_policy)
  status.total_memory_traffic=status.read_miss_count+status.write_miss_count+status.write_back_count;
  else
  status.total_memory_traffic=status.read_miss_count+status.write_count+status.write_back_count;
 
  printf("======  Simulation results (raw) ======\n");
  printf("a. number of L1 reads:%d\n",status.read_count);
  printf("b. number of L1 read misses:%d\n",status.read_miss_count);
  printf("c. number of L1 writes:%d\n", status.write_count);
  printf("d. number of L1 write misses:%d\n", status.write_miss_count);
  printf("e. L1 miss rate: %.4f \n", status.miss_rate);
  printf("f. number of writebacks from L1:%d\n",status.write_back_count);
  printf("g. total memory traffic: %d\n",status.total_memory_traffic);


}

*/

class block 
{
public:
      char tag[32];
      unsigned long timestamp;
      bool valid;
      bool dirty;
      int block_recency_count;
      double block_freq_count;
      double temp_freq_count;
      double block_aging_count;
      int last_count;
      int current_count;
      int hit;
block(int tag_size)
{
    
 for(int i=0;i<tag_size;++i)
   tag[i]='0';//initialize tag to random chars

 valid=false;
 dirty=false;
 block_recency_count=0;
 block_freq_count=0;
 block_aging_count=0;
 last_count=0;
 current_count=0;
 hit=0;
}

};


class set {
public:
      block* blocks[256];
      int set_associativity;
      float lamda;

set(int set_associativity,int tag_size)
{
  for(int i=0;i<set_associativity;++i)
  {
 //   printf("going to create block no. %d\n",i);
    blocks[i]=new block(tag_size); 
   // printf("block tag is %s\n", blocks[i]->tag);
   // printf("The block valid is %d\n",blocks[i]->valid);
  }
}  

void set_hit(int location)
{
blocks[location]->hit=1;
}

void clear_hit(int location)
{
blocks[location]->hit=0;
}

void set_valid(int location)
{
  blocks[location]->valid=true;
}

void set_current_count(int location, int count)
{
  blocks[location]->current_count=count;
}

void set_last_count(int location, int count)
{
  blocks[location]->last_count=count;
}

void set_age_count(int location)
{
  blocks[location]->block_aging_count=blocks[location]->block_freq_count;
}
/*
char get_tag(int location)
{
return blocks[location]->tag;
}
*/
void set_dirty(int location)
{
  blocks[location]->dirty = true;
}

void clear_dirty(int location)
{
blocks[location]->dirty = false;
}

int ishit(char* addrBin, int tag_size)
{
  for(int j=0;j<set_associativity;++j)
  {
    if(char_equal(blocks[j]->tag, addrBin, tag_size) && blocks[j]->valid)
    {
//	  printf("hit on set %d line %d\n",setIndex, j);
	  return j;
    }
  }
    //Give signal to processor or lower level cache that it is a miss
    return -1;
}

int emptyLineAvailable()
{
//printf("Entering %d\n",set_associativity);
  for(int j=0;j<set_associativity;++j)
  {
//  printf("The block valid is %d\n",blocks[j]->valid);
     if (blocks[j]->valid==false)
{ 
    // printf("Yes");
     return j;
} 
 }
    return -1;
}


bool check_dirty(int location)
{
    if (blocks[location]->dirty==true) 
      return true;
    else
      return false;
}


void update_recency_count(int location)
{
  if(blocks[location]->valid)
  {
    for(int i=0;i<set_associativity;++i)
    {
      if(blocks[i]->valid && (blocks[i]->block_recency_count < blocks[location]->block_recency_count))
      {
	blocks[i]->block_recency_count++;
//	blocks[i]->block_recency_count = blocks[i]->block_recency_count% set_associativity;
      }
    }
  }
  else
  {
    for(int i=0;i<set_associativity;++i)
    {
     if(blocks[i]->valid)
      {
        blocks[i]->block_recency_count++;
      }
    }
  }
blocks[location]->block_recency_count=0;//clear the age of hitted line
}  


void update_freq_count(int location)
{
if(lamda>1)
blocks[location]->block_freq_count++;
else
{
if(blocks[location]->hit==1)
{
blocks[location]->block_freq_count=(1+(pow(0.5,(lamda*(blocks[location]->current_count-blocks[location]->last_count)))*blocks[location]->block_freq_count));
//printf(" block freq count %f\n",blocks[location]->block_freq_count);
//printf(" prev count %d %d \n", blocks[location]->last_count,location);
//printf(" current count %d \n", blocks[location]->current_count);
}
else
{
blocks[location]->block_freq_count=1;
//printf(" block freq count %f\n",blocks[location]->block_freq_count);
//printf(" prev count %d %d \n", blocks[location]->last_count,location);
//printf(" current count %d \n", blocks[location]->current_count);
}
}
}

void set_tag(char* addrBin, int location, int tag_size)
{ 
    
    for(int i=0;i<tag_size;++i) 
      blocks[location]->tag[i]=addrBin[i];
    //printf("tag is %s \n", block[location]->tag);
//    status.stream_in_count++;
}

char* get_tag_index(int setindex,int location,int tag_size,int index_size)
{
   char* tag=new char[33];
   int rem,i;
   for(int i=0;i<tag_size;++i)
    tag[i]=blocks[location]->tag[i];
   char binaryNumber[32];
   for(int i=0;i<index_size;++i)
     binaryNumber[i]='0';
   for(i = 0; i < index_size; i++)
   { 
      rem = setindex % 2;
      if(rem == 0)
        binaryNumber[index_size-1-i]='0';
      else
        binaryNumber[index_size-1-i]='1';
      setindex=setindex/2;
   }
   binaryNumber[index_size]='\0';

   for(int i=tag_size;i<tag_size+index_size;++i)
     tag[i]=binaryNumber[i-(tag_size)];
   for(int i=tag_size+index_size;i<32;++i)
     tag[i]='0';
   tag[33]='\0';
   return tag;
}


int get_lru_eviction_spot()
{ 
  int location=0;
  for(int i=0;i<set_associativity;++i)
  { 
    if((blocks[i]->block_recency_count >= blocks[location]->block_recency_count))
    {
      location=i;
    }
  }  
return location;
}

int get_lfu_eviction_spot()
{ 
  int location=0;

  if(lamda>1)
  {
  for(int i=set_associativity-1;i>=0;--i)
  { 
    if((blocks[i]->block_freq_count <= blocks[location]->block_freq_count))
    {
      location=i;
    }
  }  
}

else
{
for(int i=set_associativity-1;i>=0;--i)
{
 double temp=pow(2,(lamda*(blocks[i]->last_count-blocks[i]->current_count)));
//printf("%.20f\n", temp);
blocks[i]->temp_freq_count=(double)(temp*blocks[i]->block_freq_count);
//printf(" %.100f  %d  %d\n",blocks[i]->temp_freq_count,blocks[i]->current_count,blocks[i]->last_count);
}

  for(int j=set_associativity-1;j>=0;--j)
  {
//printf("%.20f %.20f %d  ",blocks[j]->temp_freq_count,blocks[location]->temp_freq_count,location);
    if((blocks[j]->temp_freq_count) <= (blocks[location]->temp_freq_count))
    {
      location=j;
//     printf("%.20f %.20f %d  ",blocks[j]->temp_freq_count,blocks[location]->temp_freq_count,location);
    }
  }
//printf("   %d",location);
}
return location;

}
};
	
class cache {
public:
      int cache_size;
      int victim_cache_size;
      int victim_set_assoc;
      int victim_tag_size;
      int block_size;
      int set_associativity;
      int tag_size;
      int index_size;
      int num_sets;
      set* sets[65536];
      set* victim_set;
      int replacement_policy;
      int write_policy;

  int access_count;  
  int read_count; 
  int write_count; 
  int replacement_count;
  int miss_count;
  int hit_count;
  int read_miss_count;
  int write_miss_count; 
  int read_hit_count;
  int write_hit_count;
  int write_back_count; 
  double hit_rate;
  double miss_rate;
  int swaps;
  int victim_writebacks;
  int total_memory_traffic;
  float lamda;
void init_cache()
{
  num_sets=cache_size/(block_size*set_associativity);
  tag_size=(32-(int)log2(num_sets)-(int)log2(block_size));
  victim_tag_size=(32-(int)log2(block_size)); 
  index_size=(int)log2(num_sets);  
  victim_set_assoc=(victim_cache_size/block_size);
  //printf("num_sets %d\n", num_sets);
  //printf("tag_size %d\n", tag_size);
  //printf("index_size %d\n", index_size);
  //printf("victim tag size is %d\n",victim_tag_size);
  //printf("victim set associativity is %d\n",victim_set_assoc);
  access_count=0;
  read_count=0;
  write_count=0;
  replacement_count=0;
  miss_count=0;
  hit_count=0;
  read_miss_count=0;
  write_miss_count=0;
  read_hit_count=0;
  write_hit_count=0;
  write_back_count=0;
  hit_rate=0;
  miss_rate=0;
  swaps=0;
  victim_writebacks=0;  
  total_memory_traffic=0;
//  lamda=5;  
  for(int i=0;i<num_sets;++i) 
{
  // printf("going to create set no. %d\n",i);
    sets[i]= new set(set_associativity,tag_size);
    sets[i]->set_associativity=set_associativity;
    sets[i]->lamda=lamda;
}
victim_set = new set(victim_set_assoc,victim_tag_size);
victim_set->set_associativity=victim_set_assoc;
}
  
void printstatus(int write_policy)
{
  
  hit_count=read_hit_count + write_hit_count;
  hit_rate=(double) hit_count/access_count;
  miss_rate=(double) miss_count/access_count;
  if(!write_policy)
  total_memory_traffic=read_miss_count+write_miss_count+write_back_count;
  else
  total_memory_traffic=read_miss_count+write_count+write_back_count;
 
  printf("======  Simulation results (raw) ======\n");
  printf("a. number of L1 reads:%d\n",read_count);
  printf("b. number of L1 read misses:%d\n",read_miss_count);
  printf("c. number of L1 writes:%d\n", write_count);
  printf("d. number of L1 write misses:%d\n", write_miss_count);
  printf("e. L1 miss rate: %.4f \n", miss_rate);
//  printf("f. number of writebacks from L1:%d\n",write_back_count);
//  printf("g. total memory traffic: %d\n",total_memory_traffic);


}

void dump_parameter()
{
//printf("Cache size is %d\n", cache_size);
//printf("Block size is %d\n", block_size);
//printf("Set associativity is %d\n",set_associativity);
//printf("Replacement policy is %d\n", replacement_policy);
//printf("Write policy is %d\n", write_policy);
//printf("Victim cache size is %d\n",victim_cache_size);
//printf("lamda value is %f\n",lamda);
}

char* read(char* addrBin)
{   
    char* ret_addr=NULL;
  //  printf("#1: Read %x\n",bintohex(addrBin));
    int setindex = get_setindex(addrBin,tag_size,index_size);
   // printf("Current set    %d:\n", setindex);
    //printf("reading set %....\n ", setindex);
    read_count++;
    access_count++;
for(int j=0;j<set_associativity;j++)
{
sets[setindex]->set_current_count(j,access_count);
}
    int hit_location=sets[setindex]->ishit(addrBin,tag_size);
    if(hit_location!=-1)
    {
      read_hit_count++;
      //printf("L1 HIT\n");
      if(!replacement_policy)
      {
        // printf("L1 UPDATE LRU\n");
        //printf("Changed set     %d:    %x   D\n",setindex,bintohex(sets[setindex]->blocks[hit_location]->tag));
	sets[setindex]->update_recency_count(hit_location);
      }
      else
       {
     //printf("L1 UPDATE LFU\n");
       //printf("Changed set     %d:    %x   D\n",setindex,bintohex(sets[setindex]->blocks[hit_location]->tag));
        sets[setindex]->set_hit(hit_location);
        sets[setindex]->set_current_count(hit_location,access_count);
	sets[setindex]->update_freq_count(hit_location);
        sets[setindex]->set_last_count(hit_location,access_count);
        sets[setindex]->clear_hit(hit_location);
        
        //printf("%d",sets[setindex]->blocks[hit_locaiton]->freq_count);
       }
    } //if end
    else 
    { 
      read_miss_count++;
      miss_count++;
      //printf("L1 MISS\n");
      int empty_location=sets[setindex]->emptyLineAvailable();
     // printf("I am read\n");
     // printf("The empty location is %d\n",empty_location);
      if(empty_location!=-1)
      {
	//printf("R missed, space available, stream in \n");
	if(!replacement_policy)
	{  
         //printf("L1 UPDATE LRU\n");
//         printf("Changed set     %d:    D\n",setindex);
	  ret_addr=addrBin;         
	  sets[setindex]->set_tag(addrBin,empty_location,tag_size);
          //printf("Changed set     %d:    %x   D\n",setindex,bintohex(sets[setindex]->blocks[empty_location]->tag));
          sets[setindex]->update_recency_count(empty_location); 
	  sets[setindex]->set_valid(empty_location);
        }
	else
	{
        //printf("L1 UPDATE LFU\n");
        //printf("Changed set     %d:    D\n",setindex);
           ret_addr=addrBin;
	  sets[setindex]->set_tag(addrBin,empty_location,tag_size);
          //printf("Changed set     %d:    %x   D\n",setindex,bintohex(sets[setindex]->blocks[empty_location]->tag));
          sets[setindex]->set_current_count(empty_location,access_count);
          sets[setindex]->update_freq_count(empty_location);
          sets[setindex]->set_last_count(empty_location,access_count);
          sets[setindex]->set_valid(empty_location);
        }
      } //pending top else
      else 
      {
	//printf("R missed and full, do eviction\n");
	if(!replacement_policy)
	{
          //printf("L1 UPDATE LRU\n");
          if(victim_cache_size!=0)
	  {
	   // read_miss_count--;
            int temp=0;
            int location= sets[setindex]->get_lru_eviction_spot();
            int victim_hit_location=victim_set->ishit(addrBin,victim_tag_size);
            if(victim_hit_location!=-1)
            {
	      read_miss_count--;
	      swaps++;
              victim_set->set_tag((sets[setindex]->get_tag_index(setindex,location,tag_size,index_size)),victim_hit_location,victim_tag_size);
              victim_set->update_recency_count(victim_hit_location);
              if(victim_set->check_dirty(victim_hit_location))temp=1;
              if(sets[setindex]->check_dirty(location))victim_set->set_dirty(victim_hit_location);
              else victim_set->clear_dirty(victim_hit_location);
              if(temp)sets[setindex]->set_dirty(location);
              else sets[setindex]->clear_dirty(location);
            }
            else
            {
              int victim_empty_location=victim_set->emptyLineAvailable();
              if(victim_empty_location!=-1)
              {
                ret_addr=addrBin;
                victim_set->set_tag((sets[setindex]->get_tag_index(setindex,location,tag_size,index_size)),victim_empty_location,victim_tag_size);
                victim_set->update_recency_count(victim_empty_location);
                victim_set->set_valid(victim_empty_location);
                if(sets[setindex]->check_dirty(location))victim_set->set_dirty(victim_empty_location);
                sets[setindex]->clear_dirty(location);
              }
              else
              {
                int victim_location= victim_set->get_lru_eviction_spot();
                ret_addr=addrBin;
                if (victim_set->check_dirty(victim_location))
                {
		victim_writebacks++;
                 ret_addr=victim_set->get_tag_index(0,victim_location,victim_tag_size,0);
                 victim_set->clear_dirty(victim_location);
                }
                 victim_set->set_tag((sets[setindex]->get_tag_index(setindex,location,tag_size,index_size)),victim_location,victim_tag_size);
                 if(sets[setindex]->check_dirty(location))victim_set->set_dirty(victim_location);
                 sets[setindex]->clear_dirty(location);
                 victim_set->update_recency_count(victim_location);
              }
            }
          sets[setindex]->set_tag(addrBin,location,tag_size);
          sets[setindex]->update_recency_count(location);
	  }
          else
          {
          int location= sets[setindex]->get_lru_eviction_spot();
          ret_addr=addrBin;
	  if (sets[setindex]->check_dirty(location))//this is done only for wbwa
	  { 
            ret_addr=sets[setindex]->get_tag_index(setindex,location,tag_size,index_size);
//             printf("I am read %s at location %d\n",sets[setindex]->blocks[location]->tag,location);
            // printf("%s\n",ret_addr);
	    write_back_count++;
            sets[setindex]->clear_dirty(location);
	    //call the function which obtains the tag and provide higher level memory tag, set index and location to write
	  }
          //for wtna just remove the block and proceed with replacement of tag no updation is required
	  //update the cache data contents
	 // printf("Changed set     %d:     D\n",setindex);
	  sets[setindex]->set_tag(addrBin,location,tag_size);
          //printf("Changed set     %d:    %x   D\n",setindex,bintohex(sets[setindex]->blocks[location]->tag));
	  sets[setindex]->update_recency_count(location); 
          }     
        }
	else
  	{
          //printf("L1 UPDATE LFU\n");
           if(victim_cache_size!=0)
          {
//	    read_miss_count--;
            int temp=0;
            int location= sets[setindex]->get_lfu_eviction_spot();
            int victim_hit_location=victim_set->ishit(addrBin,victim_tag_size);
            if(victim_hit_location!=-1)
            {
		read_miss_count--;
		swaps++;
              victim_set->set_tag((sets[setindex]->get_tag_index(setindex,location,tag_size,index_size)),victim_hit_location,victim_tag_size);
              victim_set->update_recency_count(victim_hit_location);
              if(victim_set->check_dirty(victim_hit_location))temp=1;
              if(sets[setindex]->check_dirty(location))victim_set->set_dirty(victim_hit_location);
              else victim_set->clear_dirty(victim_hit_location);
              if(temp)sets[setindex]->set_dirty(location);
              else sets[setindex]->clear_dirty(location);
            }
            else
            {
              int victim_empty_location=victim_set->emptyLineAvailable();
              if(victim_empty_location!=-1)
              {
                ret_addr=addrBin;
                victim_set->set_tag((sets[setindex]->get_tag_index(setindex,location,tag_size,index_size)),victim_empty_location,victim_tag_size);
                victim_set->update_recency_count(victim_empty_location);
                victim_set->set_valid(victim_empty_location);
                if(sets[setindex]->check_dirty(location))victim_set->set_dirty(victim_empty_location);
                sets[setindex]->clear_dirty(location);
              }
              else
              {
                int victim_location= victim_set->get_lru_eviction_spot();
                ret_addr=addrBin;
                if (victim_set->check_dirty(victim_location))
                {
		 victim_writebacks++;
                 ret_addr=victim_set->get_tag_index(0,victim_location,victim_tag_size,0);
                 victim_set->clear_dirty(victim_location);
                }
                 victim_set->set_tag((sets[setindex]->get_tag_index(setindex,location,tag_size,index_size)),victim_location,victim_tag_size);
                 if(sets[setindex]->check_dirty(location))victim_set->set_dirty(victim_location);
                 sets[setindex]->clear_dirty(location);
	   //      victim_set->set_age_count(victim_location);
                 victim_set->update_recency_count(victim_location);
              }
            }
          sets[setindex]->set_tag(addrBin,location,tag_size);
          sets[setindex]->set_age_count(location);
          sets[setindex]->set_current_count(location,access_count);
          sets[setindex]->update_freq_count(location);
          sets[setindex]->set_last_count(location,access_count);

          }
          else
          { 
          int location= sets[setindex]->get_lfu_eviction_spot();
           ret_addr=addrBin;
	  if (sets[setindex]->check_dirty(location))
	  {
            ret_addr=sets[setindex]->get_tag_index(setindex,location,tag_size,index_size);
  //          printf("I am read %s at location %d\n",sets[setindex]->blocks[location]->tag,location);
            // printf("%s\n",ret_addr);
	    write_back_count++;
            sets[setindex]->clear_dirty(location);
	    //call the function which obtains the tag and provide higher level memory tag, set index and location to write
	  }
//	printf("Changed set     %d:    D\n",setindex);
        sets[setindex]->set_tag(addrBin,location,tag_size);
        //printf("Changed set     %d:    %x   D\n",setindex,bintohex(sets[setindex]->blocks[location]->tag));
	sets[setindex]->set_age_count(location);
        sets[setindex]->set_current_count(location,access_count);
	sets[setindex]->update_freq_count(location);
        sets[setindex]->set_last_count(location,access_count);
          }	
        }
      }
   }
/*
for(int j=0;j<set_associativity;j++)
{
if(sets[setindex]->blocks[j]->dirty)
printf("  %x   D  %f  %d %d",bintohex(sets[setindex]->blocks[j]->tag),sets[setindex]->blocks[j]->block_freq_count, sets[setindex]->blocks[j]->current_count, sets[setindex]->blocks[j]->last_count);
else
printf("  %x      %f %d %d",bintohex(sets[setindex]->blocks[j]->tag),sets[setindex]->blocks[j]->block_freq_count, sets[setindex]->blocks[j]->current_count, sets[setindex]->blocks[j]->last_count);
}
printf("\n");
*/

/*
for(int j=0;j<vicset_assoc;j++)
{
if(victim_set->blocks[j]->dirty)
printf("  %x   D %f   ",bintohex(sets->blocks[j]->tag),sets->blocks[j]->block_freq_count);
else
printf("  %x %f ",bintohex(sets->blocks[j]->tag),sets->blocks[j]->block_freq_count);
}
printf("\n");
*/
return ret_addr;

}

char* write(char* addrBin)
{
    char* ret_addr=NULL;
   // printf("#1: Write %x\n",bintohex(addrBin));
    int setindex = get_setindex(addrBin,tag_size,index_size);
//   printf("%X\n",bintohex(addrBin));
  //  printf("Current set    %d:\n", setindex);
    write_count++;
    access_count++;
for(int j=0;j<set_associativity;j++)
{
sets[setindex]->set_current_count(j,access_count);
}
   //printf("I am write\n");
    int hit_location=sets[setindex]->ishit(addrBin,tag_size);
    //printf("L1 Write:%s(tag %p, index %d\n)",addrBin,sets[setindex]->blocks[hit_location]->tag,setindex);
    //printf("Current set    %d:\n", setindex);
    if(hit_location!=-1)
    {
      write_hit_count++;
      //printf("L1 HIT\n");
      if(!replacement_policy)
      {
        //printf("L1 UPDATE LRU\n");
	sets[setindex]->update_recency_count(hit_location);
        if(!write_policy)
        {//update the cache data contents for wbwa
	  //printf("L1 SET DIRTY\n");
          //printf("Changed set     %d:    %x   D\n",setindex,bintohex(sets[setindex]->blocks[hit_location]->tag));
       //  sets[setindex]->update_recency_count(hit_location);
	  sets[setindex]->set_dirty(hit_location);
        }
        else
        {
         ;//update the cache data contents
          //call the function which provide addrbin to higher level memory for updation for wtna
	}
      }
      else
      {
      //  printf("L1 UPDATE LFU\n");
        sets[setindex]->set_hit(hit_location);
        sets[setindex]->set_current_count(hit_location,access_count);
	sets[setindex]->update_freq_count(hit_location);
        sets[setindex]->set_last_count(hit_location,access_count);
        sets[setindex]->clear_hit(hit_location);
        if(!write_policy)
        {//update the cache data contents
	//  printf("L1 SET DIRTY\n");
          //printf("Changed set     %d:    %X   D\n",setindex,bintohex(sets[setindex]->blocks[hit_location]->tag));
         // sets[setindex]->update_freq_count(hit_location);
	  sets[setindex]->set_dirty(hit_location);
        }
        else
        {
         ; //update the cache data contents
           //call the function which provide addrbin to higher level memory for updation for wtna
	}
      }
    } 
    else 
    { 
      write_miss_count++;
      miss_count++;
      //printf("L1 MISS\n");
      int empty_location=sets[setindex]->emptyLineAvailable();
      //printf("Changed set     %d:    D\n",setindex);
//      printf("location is %d", empty_location);
      if(!write_policy)//for wbwa
      {
        if(empty_location!=-1)
        {
//	  printf("W missed, space available, stream in \n");
	  if(!replacement_policy)
	  {
            ret_addr=addrBin;
            //printf("L1 UPDATE LRU\n");
	    sets[setindex]->set_tag(addrBin,empty_location,tag_size);
	    //update the cache data contents
            sets[setindex]->update_recency_count(empty_location); 
	    sets[setindex]->set_valid(empty_location);
	    //printf("L1 SET DIRTY\n");
            //printf("Changed set     %d:    %x   D\n",setindex,bintohex(sets[setindex]->blocks[empty_location]->tag));
	    sets[setindex]->set_dirty(empty_location);
          }
	  else
	  {
            ret_addr=addrBin;
        //    printf("L1 UPDATE LFU\n");
	    sets[setindex]->set_tag(addrBin,empty_location,tag_size);
	    //update the cache contents
	    sets[setindex]->set_current_count(empty_location,access_count);
            sets[setindex]->update_freq_count(empty_location);
            sets[setindex]->set_last_count(empty_location,access_count);
            sets[setindex]->set_valid(empty_location);
	  //  printf("L1 SET DIRTY\n");
            //printf("Changed set     %d:    %X   D\n",setindex,bintohex(sets[setindex]->blocks[empty_location]->tag));
            sets[setindex]->set_dirty(empty_location);
          }
        } 
        else 
        {
//	  printf("W missed and full, do eviction\n");
	  if(!replacement_policy)
	  {
          if(victim_cache_size!=0)
          { 
	  //  write_miss_count--;
            int temp=0;
            int location= sets[setindex]->get_lru_eviction_spot();
            int victim_hit_location=victim_set->ishit(addrBin,victim_tag_size);
            if(victim_hit_location!=-1)
            {
		write_miss_count--;
		swaps++;
              victim_set->set_tag((sets[setindex]->get_tag_index(setindex,location,tag_size,index_size)),victim_hit_location,victim_tag_size);
              victim_set->update_recency_count(victim_hit_location);
              if(victim_set->check_dirty(victim_hit_location))temp=1;
              if(sets[setindex]->check_dirty(location))victim_set->set_dirty(victim_hit_location);
              else victim_set->clear_dirty(victim_hit_location);
              if(temp)sets[setindex]->set_dirty(location);
              else sets[setindex]->clear_dirty(location);
            }
            else
            {
              int victim_empty_location=victim_set->emptyLineAvailable();
              if(victim_empty_location!=-1)
              {
                ret_addr=addrBin;
                victim_set->set_tag((sets[setindex]->get_tag_index(setindex,location,tag_size,index_size)),victim_empty_location,victim_tag_size);
                victim_set->update_recency_count(victim_empty_location);
                victim_set->set_valid(victim_empty_location);
                if(sets[setindex]->check_dirty(location))victim_set->set_dirty(victim_empty_location);
                sets[setindex]->clear_dirty(location);
              }
              else
              {
                int victim_location= victim_set->get_lru_eviction_spot();
                ret_addr=addrBin;
                if (victim_set->check_dirty(victim_location))
                {
		victim_writebacks++;
                 ret_addr=victim_set->get_tag_index(0,victim_location,victim_tag_size,0);
                 victim_set->clear_dirty(victim_location);
                }
                 victim_set->set_tag((sets[setindex]->get_tag_index(setindex,location,tag_size,index_size)),victim_location,victim_tag_size);
                 victim_set->update_recency_count(victim_location);
                 if(sets[setindex]->check_dirty(location))victim_set->set_dirty(victim_location);
                 sets[setindex]->clear_dirty(location);
              }
            }
          sets[setindex]->set_tag(addrBin,location,tag_size);
          sets[setindex]->update_recency_count(location);
	  sets[setindex]->set_dirty(location);
          }
           else
           { 
	    //printf("L1 UPDATE LRU\n");
            int location= sets[setindex]->get_lru_eviction_spot();
             ret_addr=addrBin;
	    if (sets[setindex]->check_dirty(location))
	    {
               ret_addr=sets[setindex]->get_tag_index(setindex,location,tag_size,index_size);
           //     printf("I am write %s at location %d\n",sets[setindex]->blocks[location]->tag,location);
              // printf("%s\n",ret_addr);
	      write_back_count++;
	    //call the function which obtains the tag and provide higher level memory tag, set index and location to write
	    }
	    sets[setindex]->set_tag(addrBin,location,tag_size);
            //update the cache data contents
            //printf("L1 SET DIRTY\n");
            //printf("Changed set     %d:    %x   D\n",setindex,bintohex(sets[setindex]->blocks[location]->tag));
	    sets[setindex]->update_recency_count(location);
            sets[setindex]->set_dirty(location);
            }
          }
	  else
  	  {
           if(victim_cache_size!=0)
          { 
	   // write_miss_count--;
            int temp=0;
            int location= sets[setindex]->get_lfu_eviction_spot();
            int victim_hit_location=victim_set->ishit(addrBin,victim_tag_size);
            if(victim_hit_location!=-1)
            {
		write_miss_count--;
		swaps++;
              victim_set->set_tag((sets[setindex]->get_tag_index(setindex,location,tag_size,index_size)),victim_hit_location,victim_tag_size);
              victim_set->update_recency_count(victim_hit_location);
              if(victim_set->check_dirty(victim_hit_location))temp=1;
              if(sets[setindex]->check_dirty(location))victim_set->set_dirty(victim_hit_location);
              else victim_set->clear_dirty(victim_hit_location);
              if(temp)sets[setindex]->set_dirty(location);
              else sets[setindex]->clear_dirty(location);
            }
            else
            {
              int victim_empty_location=victim_set->emptyLineAvailable();
              if(victim_empty_location!=-1)
              {
                ret_addr=addrBin;
                victim_set->set_tag((sets[setindex]->get_tag_index(setindex,location,tag_size,index_size)),victim_empty_location,victim_tag_size);
                victim_set->update_recency_count(victim_empty_location);
                victim_set->set_valid(victim_empty_location);
                if(sets[setindex]->check_dirty(location))victim_set->set_dirty(victim_empty_location);
                sets[setindex]->clear_dirty(location);
              }
              else
              {
                int victim_location= victim_set->get_lru_eviction_spot();
                ret_addr=addrBin;
                if (victim_set->check_dirty(victim_location))
                {
		 victim_writebacks++;
                 ret_addr=victim_set->get_tag_index(0,victim_location,victim_tag_size,0);
                 victim_set->clear_dirty(victim_location);
                }
                 victim_set->set_tag((sets[setindex]->get_tag_index(setindex,location,tag_size,index_size)),victim_location,victim_tag_size);
   //              victim_set->set_age_count(victim_location);
                 victim_set->update_recency_count(victim_location);
                 if(sets[setindex]->check_dirty(location))victim_set->set_dirty(victim_location);
                 sets[setindex]->clear_dirty(location);
              }
            }
          sets[setindex]->set_tag(addrBin,location,tag_size);
          sets[setindex]->set_age_count(location);
          sets[setindex]->set_current_count(location,access_count);
	  sets[setindex]->update_freq_count(location);
          sets[setindex]->set_last_count(location,access_count);
	  sets[setindex]->set_dirty(location);
          }
            else
            {
            //printf("L1 UPDATE LFU\n");
            int location= sets[setindex]->get_lfu_eviction_spot();
             ret_addr=addrBin;
	    if (sets[setindex]->check_dirty(location))
	    {
              ret_addr=sets[setindex]->get_tag_index(setindex,location,tag_size,index_size);
            //  printf("I am write %s at location %d\n",sets[setindex]->blocks[location]->tag,location);
	     // printf("%s\n",ret_addr);
	      write_back_count++;
	      //call the function which obtains the tag and provide higher level memory tag, set index and location to write
	    }
	    sets[setindex]->set_tag(addrBin,location,tag_size);
            //update the cache data contents
            //printf("L1 SET DIRTY\n");
            //printf("Changed set     %d:    %x   D\n",setindex,bintohex(sets[setindex]->blocks[location]->tag));
	    sets[setindex]->set_age_count(location);
            sets[setindex]->set_current_count(location,access_count);
	    sets[setindex]->update_freq_count(location);
            sets[setindex]->set_last_count(location,access_count);
            sets[setindex]->set_dirty(location);
            }	
          }
        }
      }
      else
      {
      ;//call the function which provide addrbin to higher level memory for updation
      }
   }
/*
for(int j=0;j<set_associativity;j++)
{
if(sets[setindex]->blocks[j]->dirty)
printf("  %x   D  %f %d %d",bintohex(sets[setindex]->blocks[j]->tag),sets[setindex]->blocks[j]->block_freq_count, sets[setindex]->blocks[j]->current_count, sets[setindex]->blocks[j]->last_count);
else
printf("  %x      %f %d %d",bintohex(sets[setindex]->blocks[j]->tag),sets[setindex]->blocks[j]->block_freq_count, sets[setindex]->blocks[j]->current_count, sets[setindex]->blocks[j]->last_count);
}
printf("\n");
*/

/*
for(int j=0;j<victim_set_assoc;j++)
{
if(victim_set->blocks[j]->dirty)
printf("  %x   D %f  ",bintohex(sets->blocks[j]->tag),sets->blocks[j]->block_freq_count);
else
printf("  %x %f      ",bintohex(sets->blocks[j]->tag),sets->blocks[j]->block_freq_count);
}
printf("\n");
*/
return ret_addr;

}      
    
void set_cache_param(int param, int value)
{
  switch (param) 
   {
     case 1:
      block_size = value;
//      printf("did");
     break;
     case 2:
      cache_size = value;
//      printf("did");
     break;
     case 3:
      set_associativity = value;
//      printf("did");
     break;
     case 4:
      replacement_policy = value;
//      printf(value);
     break;
     case 5:
       write_policy = value;
     break;
     case 6:
       victim_cache_size=value;
     break;
     default:
       printf("error set_cache_param: bad parameter value\n");
       exit(-1);
  }
}

};

int is_power_of_2(int i) {
    if ( i <= 0 ) 
    {
        return 0;
    }
    return ! (i & (i-1));
}

bool parameter_sanity_check( char* argv[])
{
int arg=0;
float value=0;
float temp=0;
for(arg=1; arg<9; arg++)
{
     if(arg==1) 
     {
        printf("  L1_BLOCKSIZE:    %s\n",argv[arg]);
	value = atoi(argv[arg]);
        if(is_power_of_2(value)==0)
        {
	  printf("The cache block size is not power of two\n");
	  return false;
        }
        continue;
     }
     if(arg==2)
     {
	printf("  L1_SIZE:     %s\n",argv[arg]);
	value = atoi(argv[arg]);
        if(is_power_of_2(value)==0)
        {
	  printf("The cache size is not power of two\n");
          return false;
        } 
        continue;
     }
 
    if(arg==3) 
    {
	printf("  L1_ASSOC:     %s\n",argv[arg]);
	value = atoi(argv[arg]);
        continue;
    }
/*
    if(arg==4) 
    {
	if(*argv[arg]==0)
        {
	  printf("  L1_REPLACEMENT_POLICY:   %s\n",argv[arg]);
        }
	else
        {
	  value = atoi(argv[arg]);
	  printf("  L1_REPLACEMENT_POLICY:   %s\n",argv[arg]);
        }
        continue;
     }
*/

    if(arg==4)
    {
        printf("Victim_Cache_SIZE:     %s\n",argv[arg]);
        value = atoi(argv[arg]);
        if(is_power_of_2(value)==0 && value!=0)
        {
          printf("The cache size is not power of two\n");
          return false;
        }

        continue;
    }
/*
    if(arg==5) 
    {
	if(*argv[arg]==0)
        {
	  printf("  L1_WRITE_POLICY:    %s\n",argv[arg]);
        }
	else
        {
	  printf("  L1_WRITE_POLICY:    %s\n",argv[arg]);
        }
      continue;
    }
*/
     if(arg==5)
     {
        printf("  L2_SIZE:     %s\n",argv[arg]);
        value = atoi(argv[arg]);
        if(is_power_of_2(value)==0 && value!=0)
        {
          printf("The cache size is not power of two\n");
          return false;
        }
        continue;
     }

    if(arg==6)
    {
        printf("  L2_ASSOC:     %s\n",argv[arg]);
        value = atoi(argv[arg]);
        continue;
    }

  if(arg==7)
    {

        //printf("  Replacement Policy:   %s\n",argv[arg]);
          value = atof(argv[arg]);
	  temp=value;
  //      printf("value:%d\n",value);
         if(value!=2 && value!=3 && !(value>=0 && value<=1))
	{
          printf("  Your replacement policy input is not correct   \n");
          return false;
        }
        continue;
     }

  if(arg==8) 
    {
      printf("    trace_file:    %s\n",argv[arg]);
      if(temp==2)
      printf("  Replacement Policy:  LRU \n");
      if(temp==3)
      printf("  Replacement Policy:  LFU \n");
      if(temp>=0 && temp<=1)
{
      printf("  Replacement Policy:  LRFU \n"); 
      printf("  lambda: %.2f\n",temp);
}
      continue;
    }  
}
return true;
}

char* hexToBin(char* in){ 
    int x = 4;
    int size;
    size = strlen(in);
   //printf("size is %d\n",size);
   char input[]="00000000";
    int i;
    for (i = 0; i < size + 1; i++) 
    {
        input[8-size+i] = in[i];
    }

    //printf("buffer : %s\n", input);

    char* output = new char[8*4+1];
   
    for (int i = 0; i < 8; i++)
    {
        if (input[i] =='0') {
            output[i*x +0] = '0';
            output[i*x +1] = '0';
            output[i*x +2] = '0';
            output[i*x +3] = '0';
        }
        else if (input[i] =='1') {
            output[i*x +0] = '0';
            output[i*x +1] = '0';
            output[i*x +2] = '0';
            output[i*x +3] = '1';
        }    
        else if (input[i] =='2') {
            output[i*x +0] = '0';
            output[i*x +1] = '0';
            output[i*x +2] = '1';
            output[i*x +3] = '0';
        }    
        else if (input[i] =='3') {
            output[i*x +0] = '0';
            output[i*x +1] = '0';
            output[i*x +2] = '1';
            output[i*x +3] = '1';
        }    
        else if (input[i] =='4') {
            output[i*x +0] = '0';
            output[i*x +1] = '1';
            output[i*x +2] = '0';
            output[i*x +3] = '0';
        }    
        else if (input[i] =='5') {
            output[i*x +0] = '0';
            output[i*x +1] = '1';
            output[i*x +2] = '0';
            output[i*x +3] = '1';
        }    
        else if (input[i] =='6') {
            output[i*x +0] = '0';
            output[i*x +1] = '1';
            output[i*x +2] = '1';
            output[i*x +3] = '0';
        }    
        else if (input[i] =='7') {
            output[i*x +0] = '0';
            output[i*x +1] = '1';
            output[i*x +2] = '1';
            output[i*x +3] = '1';
        }    
        else if (input[i] =='8') {
            output[i*x +0] = '1';
            output[i*x +1] = '0';
            output[i*x +2] = '0';
            output[i*x +3] = '0';
        }
        else if (input[i] =='9') {
            output[i*x +0] = '1';
            output[i*x +1] = '0';
            output[i*x +2] = '0';
            output[i*x +3] = '1';
        }
        else if (input[i] =='a') {    
            output[i*x +0] = '1';
            output[i*x +1] = '0';
            output[i*x +2] = '1';
            output[i*x +3] = '0';
        }
        else if (input[i] =='b') {
            output[i*x +0] = '1';
            output[i*x +1] = '0';
            output[i*x +2] = '1';
            output[i*x +3] = '1';
        }
        else if (input[i] =='c') {
            output[i*x +0] = '1';
            output[i*x +1] = '1';
            output[i*x +2] = '0';
            output[i*x +3] = '0';
        }
        else if (input[i] =='d') {    
            output[i*x +0] = '1';
            output[i*x +1] = '1';
            output[i*x +2] = '0';
            output[i*x +3] = '1';
        }
        else if (input[i] =='e'){    
            output[i*x +0] = '1';
            output[i*x +1] = '1';
            output[i*x +2] = '1';
            output[i*x +3] = '0';
        }
        else if (input[i] =='f') {
            output[i*x +0] = '1';
            output[i*x +1] = '1';
            output[i*x +2] = '1';
            output[i*x +3] = '1';
        }
    }

    output[32] = '\0';
    //printf("strlen of output is %d\n",strlen(output));
    return output;
}




int main(int argc, char *argv[]) 

{

 cache* L1_cache=new cache();
 cache* L2_cache=new cache();
char* return_addr_L1=NULL;
char* return_addr_L2=NULL;
 float value=0;
int l2_cache_size=0;
int config=0;
 if(argc !=9)
 {
    printf("You have either not given the full parameters or some unknown parameter is recognized. The cache won't initialize\n");
    exit(-1);
 }
    printf("===== Simulator configuration =====\n");
 if(parameter_sanity_check( argv))
 {  
    value = atoi(argv[1]);
    L1_cache->set_cache_param(1, value);
    L2_cache->set_cache_param(1, value);
    value = atoi(argv[2]);
    L1_cache->set_cache_param(2, value);
    value = atoi(argv[3]);
    L1_cache->set_cache_param(3, value);
    value = atoi(argv[4]);
    L1_cache->set_cache_param(6, value);
    value = atoi(argv[5]);
    l2_cache_size=value;
    L2_cache->set_cache_param(2, value);
    value = atoi(argv[6]);
    L2_cache->set_cache_param(3, value);
    value = atof(argv[7]);
    if(value==2 || value==3)
    {
    L1_cache->set_cache_param(4, value-2);
    L2_cache->set_cache_param(4, value-2); 
    }
    else
    {
    L1_cache->set_cache_param(4,1);
    L2_cache->set_cache_param(4,0);
    }
    L1_cache->set_cache_param(5, 0);
    L2_cache->set_cache_param(5, 0);
    L2_cache->set_cache_param(6, 0);
//   L1_cache->dump_parameter();
 //  L2_cache->dump_parameter();
   
    L1_cache->lamda=value;
    L2_cache->lamda=5;
    L1_cache->init_cache();
 //  L1_cache->lamda=value;

    if(l2_cache_size!=0)
    L2_cache->init_cache();
   
    if(l2_cache_size==0 )
     config=0;
    
    if(l2_cache_size!=0)
     config=1;

   //L1_cache->dump_parameter();
   //L2_cache->dump_parameter();

//exit(-1);

 //   printf("The cache initialized\n");

   char* fileString;

   fileString = argv[argc-1];
   FILE* inFile = freopen(fileString,"r",stdin);
   if(inFile==NULL)
   {
    printf("PLEASE CHECK YOUR FILE \n \n");
   }
   else
   {
  // printf("FILE SUCCESSFULLY LOADED \n");
   int count=1;
   while(true)
   { 
     char temp[2]; //to store r/w indicator
     fscanf(stdin,"%s",temp);//scan them into temp[]
	char temp2[8];//to store address starting with 0x..
	fscanf(stdin,"%s",temp2);
	//printf("size of temp: %d\n",sizeof(temp));
	if(feof(stdin)) break;
	if(!strcmp(temp,"r")||!strcmp(temp,"R"))
         {

         if(config==0)
         L1_cache->read(hexToBin(&temp2[0]));

         if(config==1)
        {
	// printf("# %d Read: %s\n",count,temp2);
	    return_addr_L1 = L1_cache->read(hexToBin(&temp2[0]));
            //printf("I am actual address%s\n",return_addr_L1);
         if(return_addr_L1!=NULL)
	{
         if(strcmp(return_addr_L1,(hexToBin(&temp2[0])))==0)
        {
	   //printf("Entered\n");
            return_addr_L2=L2_cache->read(hexToBin(&temp2[0]));
        }
         else
	{
           // printf("I am address%s\n",return_addr_L1);
            return_addr_L2=L2_cache->write(return_addr_L1);
            return_addr_L2=L2_cache->read(hexToBin(&temp2[0]));
         }   
	}
        }
	}
	else if(!strcmp(temp,"w")||!strcmp(temp,"W"))
        {
         if(config==0)
         L1_cache->write(hexToBin(&temp2[0]));
      
        if(config==1)
        {
	 //printf("# %d Write: %s\n",count,temp2);
	 return_addr_L1 = L1_cache->write(hexToBin(&temp2[0]));
         // printf("I am actual address%s\n",return_addr_L1);
         if(return_addr_L1!=NULL)
        {
         if(strcmp(return_addr_L1,(hexToBin(&temp2[0])))==0)
         {
           // printf("Entered\n");
            return_addr_L2=L2_cache->read(hexToBin(&temp2[0]));
         }
         else
        {
           // printf("I am address%s\n",return_addr_L1);
            return_addr_L2=L2_cache->write(return_addr_L1);
            return_addr_L2=L2_cache->read(hexToBin(&temp2[0]));
        }
        }
        }
	}
	//report strange character in input if found
	else {
	  printf("CHECK YOUR INPUT..Character not identified\n");
        exit(-1);
	}
	      count++;
      }//end-while-loop

      fclose(stdin);//close the stdin stream
/*
for(int i=0;i<1024;i++)
{
printf("set%d:  %X %d\n",i,bintohex(L1_cache->sets[i]->blocks[0]->tag),mycache->sets[i]->blocks[0]->dirty);
}
*/
printf("===================================");
printf("\n\n===== L1 contents =====\n");

for(int i=0;i<L1_cache->num_sets;i++)
{
printf("set%d:",i);
for(int j=0;j<L1_cache->set_associativity;j++)
{
if(L1_cache->sets[i]->blocks[j]->dirty)
printf("  %x   D   ",bintohex(L1_cache->sets[i]->blocks[j]->tag));
else
printf("  %x       ",bintohex(L1_cache->sets[i]->blocks[j]->tag));
}
printf("\n");
}

if(L1_cache->victim_cache_size!=0)
{
printf("===== Victim Cache contents =====\n");

printf("set 0:");
for(int j=0;j<L1_cache->victim_set_assoc;j++)
{
if(L1_cache->victim_set->blocks[j]->dirty)
printf(" %x    D   ",bintohex(L1_cache->victim_set->blocks[j]->tag));
else
printf(" %x        ",bintohex(L1_cache->victim_set->blocks[j]->tag));
}
printf("\n");
}
if(l2_cache_size!=0)
printf("===== L2 contents =====\n");

for(int i=0;i<L2_cache->num_sets;i++)
{
printf("set%d:",i);
for(int j=0;j<L2_cache->set_associativity;j++)
{
if(L2_cache->sets[i]->blocks[j]->dirty)
printf("  %x   D   ",bintohex(L2_cache->sets[i]->blocks[j]->tag));
else
printf("  %x       ",bintohex(L2_cache->sets[i]->blocks[j]->tag));
}
printf("\n");
}


L1_cache->hit_count=L1_cache->read_hit_count + L1_cache->write_hit_count;
L1_cache->hit_rate=(double) L1_cache->hit_count/L1_cache->access_count;
L1_cache->miss_rate=(double) (L1_cache->read_miss_count+L1_cache->write_miss_count)/L1_cache->access_count;
L2_cache->hit_count=L2_cache->read_hit_count + L2_cache->write_hit_count;
L2_cache->hit_rate=(double) L2_cache->hit_count/L2_cache->access_count;
L2_cache->miss_rate=(double) (L2_cache->read_miss_count+L2_cache->write_miss_count)/L2_cache->access_count;
L1_cache->total_memory_traffic=L1_cache->read_miss_count+L1_cache->write_miss_count+L1_cache-> victim_writebacks;
L2_cache->total_memory_traffic=L2_cache->read_miss_count+L2_cache->write_miss_count+L2_cache->write_back_count;
printf("\n======  Simulation results (raw) ======\n\n");
printf("a. number of L1 reads:%d\n",L1_cache->read_count);
printf("b. number of L1 read misses:%d\n",L1_cache->read_miss_count);
printf("c. number of L1 writes:%d\n",L1_cache->write_count);
printf("d. number of L1 write misses:%d\n",L1_cache->write_miss_count);
printf("e. L1 miss rate: %.4f \n",L1_cache->miss_rate);
//printf("f. number of writebacks from L1:%d\n",write_back_count);
//printf("g. total memory traffic: %d\n",total_memory_traffic)
printf("f. number of swaps:     %d\n",L1_cache->swaps);
printf("g. number of victim cache writeback:   %d\n",L1_cache->victim_writebacks);
printf("h. number of L2 reads:%d\n",L2_cache->read_count);
printf("i. number of L2 read misses:%d\n",L2_cache->read_miss_count);
printf("j. number of L2 writes:%d\n",L2_cache->write_count);
printf("k. number of L2 write misses:%d\n",L2_cache->write_miss_count);
if(l2_cache_size!=0)
printf("l. L2 miss rate: %.4f \n",L2_cache->miss_rate);
else
printf("l. L2 miss rate:   0\n");
printf("m. number of L2 writeback:%d\n",L2_cache->write_back_count);
if(l2_cache_size!=0)
printf("n. total memory traffic: %d\n",L2_cache->total_memory_traffic);
else
printf("n. total memory traffic: %d\n",L1_cache->total_memory_traffic);
//printf("f. number of swaps:     %d\n",swaps);
//printf("g. number of victim cache writeback:   %d\n",L1_cache->victim_writebacks);




double temp=(2.5*((L1_cache->cache_size)/(512.00*1024.00)));
double temp2=(2.5*((L2_cache->cache_size)/(512.00*1024.00)));
//printf("%f",temp);
double L1_hit_time = 0.25 + temp + (0.025*(L1_cache->block_size/16)) + (0.025*L1_cache->set_associativity);
double L2_hit_time = 2.5  + temp2 + (0.025*(L2_cache->block_size/16)) + (0.025*L2_cache->set_associativity);
//printf("%f",hit_time);
double L1_miss_penalty = 20 + (0.5*(L1_cache->block_size / 16));
double L2_miss_penalty = 20 + (0.5*(L2_cache->block_size / 16));
printf("\n  ==== Simulation results (performance) ====\n");
if(l2_cache_size!=0)
printf("1. average access time:      %.4f ns\n",(L1_hit_time+(L1_cache->miss_rate*(L2_hit_time+(L2_cache->miss_rate*L2_miss_penalty)))));
else
printf("1. average access time:      %.4f ns\n",(L1_hit_time+(L1_cache->miss_rate*L1_miss_penalty)));
//    char p[9]="400341a0";
  //  *p="400341a0";
 //   L1_cache->write(hexToBin(&p[0]));

 }
}
 
 else
 {
    printf("The cache won't initialize\n");
    exit(-1);
 }
}
