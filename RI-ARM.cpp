#include <iostream>
#include <vector>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include <sys/time.h>
#include <unistd.h>
#include<math.h>
using namespace std;

/****************Declaration of Variables and Structures***************************/
#define N 1000 // represents the number of documents in the collection
#define M 31  //represents the number of terms in the collection
#define packets 2 // represents the number of packets using in the ARM process
# define alpha 0.5 // represents the weight of the support in the first fitness function
# define Beta 0.5 // represents the weight of the confidence in the first fitness function
int k=5; // represents the number of bees on the colony
int IMAX=1; // the maximum number of iteration of the BSO algorithm
/*******structures needed for the documents base******/
int pertinence[N];
typedef struct aa{
char content[20]; 
}index_terms;
index_terms hash_table[M];
typedef struct aaa{
char content[20];
int occurence; 
}term;
typedef struct bbb{
int ID_doc;
term list_term[M];
}entry;
vector<entry> list_doc(N);
int req[M]; // table of user's request
float relevent_docs1[N]; // includes the score of each document according to the given request by classical RI
float relevent_docs2[N]; // includes the score of each document according to the given request by ARM
/*********structures needed for the rules base **********/
typedef struct ccc{
int Items[M];
}transaction;
vector<transaction> dataset(N);
typedef struct ddd{
int packet; 
int rule[M];
}rules;
vector<rules> rules_base(1000);
typedef struct eeee{int solution[M]; float cost;} bee;
bee Sref;
vector<bee> T_Dance(k);
int  flip=1; 
int index=0;
/*******Declaration of prototypes**************/
/***********commmon functions*******/
void copy(int t[], int v[]); /**********************copry t in v********/
bool strcmp_new(char t[],char v[]); // compare two string if equal we return  0.
/*******RI prototypes*****/
void init_struct(); /*initialize structures ************/
void read_collection(); /*read colection from the file and insert it in the matrix_doc*/
void classical_RI_process(); /*** search the relevant documents according to the given requets using classical method********/
void ARM_RI_process(int packet); /*** search the relevant documents according to the given requets using classical method********/
/*****ARM Prototypes***********/
void transformation_transactions(); /********transformation to transactional base******/
void create_Sref(int packet); // here we create the solution reference sref and initialize it with the random way
float support_rule(int s[], int packet); // this function calculates the support of the entire solution s
float support_antecedent(int s[], int packet); // this function computes the support of the antecedent of the solution s
float support_consequent(int s[], int packet); // this function computes the support of the consequent of the solution s
float confidence(int s[], int packet); // it calculates the confidence of the rule
float lift(int s[], int packet); // it calculates the lift of the rule
float leverage(int s[], int packet); // it calculates the leverage of the rule 
float coverage(int s[], int packet); // it calculates the coverage of the rule
float fitness1(int s[], int packet); // computes the fitness of a given solution s
bee neighborhood_computation(bee S, int packet);// this function explores the local region for each bee
void search_area(bee s, int packet); //detremines the search area for each bee
int best_dance(); // return the best dance after the exploration of search region of each bee
void ARM_process(int packet); /**********exctract rules for each packet of documents**********/
void stored_rules(int packet); // store the rules of a given packet in rules base from a given index
int scores(int packet); // computes the score between the packet "pk" and the user's request "req"
/******display functions*******/
void display_struct(); /*********display documents**********/
void display_transactions(); /*********************display_transactional database******/
void display_solution(bee S, int packet); // this function display the current solution with its cost
void display_rules(); // this function allow to display all obtained rules having a support no nil
void display_request(); // display the request in the rule form
void display_relevent_docs(); // display relevent documents
float efficiency();/***************efficiency***************************/
/*****************************************Main program*******************************************************************************************/
int main()
{   
    int x;
    init_struct();
    read_collection();
    display_request();
    //display_struct();
    transformation_transactions();
    /*create_Sref();
    display_solution(Sref);
    printf ("\nsupport of antecedent part is: %f\n",support_antecedent(Sref.solution));
    printf ("\nsupport of consequent part is: %f\n",support_consequent(Sref.solution));
    printf ("\nsupport of the rule is: %f\n",support_rule(Sref.solution));
    printf ("\nconfidence of the rule is: %f\n",confidence(Sref.solution));
    printf ("\nfitness of the rule is: %f\n",Sref.cost);
    search_area(Sref);
    bee result=neighborhood_computation(T_Dance[0]);
    printf ("\n the fitness of the best neighbor is: %f\n", result.cost); 
    display_solution(result);
    display_transactions();*/
   
    int max_scores=0;
    int max_packet=0;
    for (int pk=1;pk<=packets;pk++)
       {
             ARM_process(pk);
             stored_rules(pk);
       }
        clock_t t5,t6;
     t5=clock();      
       for (int pk=1;pk<=packets;pk++)
       {
            x=scores(pk);
             if (x>max_scores)
               {
                max_scores=x;
                max_packet=pk;                    
               } 
       }
     t6=clock();
     float diff1 =((float)t6-(float)t5);
     float seconds1 = diff1 / CLOCKS_PER_SEC;
         printf ("The packet giving maximum gain is %d th packet and its scores is:%d\n",max_packet,max_scores);
         
    //display_rules();
    //printf("\n%d\n", index);
    //display_request();
    clock_t t1,t2;
     t1=clock();
    classical_RI_process();
    t2=clock();
     float diff =((float)t2-(float)t1);
     float seconds = diff / CLOCKS_PER_SEC;
     printf ("\n execution time: %f", seconds);
    clock_t t3,t4;
     t3=clock(); 
     ARM_RI_process(max_packet);
     t4=clock();
     float diff2 =((float)t4-(float)t3);
     float seconds2 =seconds1+ (diff2 / CLOCKS_PER_SEC);
     printf ("\n execution time: %f", seconds2);
    //display_relevent_docs();
    printf("\n%f", efficiency());
    scanf("%d",&x);
return 0;    
}
/***********************initialize structure******/ 
void init_struct()
{
for (int i=0;i<N;i++)
{
    for (int j=0;j<M;j++)
     {
     list_doc[i].list_term[j].occurence=0;
     }
     relevent_docs1[i]=0;
     relevent_docs2[i]=0;
}  
for (int j=0;j<M;j++)
     {
     rules_base[j].packet=0;
     req[j]=0;
     }   
}
/*****************************the bodies of procedures***************/
bool strcmp_new(char t[],char v[])
{
int i=0;
bool equal=true;
while (i<20 && t[i]!='\0' && v[i]!='\0' && equal==true)
{
if (t[i]!=v[i]) equal=false;
i++;
}
return equal;
}
/****RI prototypes*********/
void read_collection()
{
int c ;
char t[20];
int j;
int i=0;
int l=0;
FILE *f=NULL;
FILE *fp=NULL;
FILE *fr=NULL;
/*************for the terms file*******/
fp=fopen("C://Users//mobntic//Desktop//generator//terme.txt","r");
if (fp!=NULL)
{
    j=0;
    int idx=0;
       while (c!=EOF)
         {
          c=fgetc(fp); 
          if (c=='\n' || c==EOF){
             t[j]='\0'; 
             strcpy(hash_table[idx].content, t);
             j=0;
             idx++;
                    }
          if (c!='\n'){t[j]=c;j++;}
          }    
}
fclose(fp);
/*******************for the request file*****/
fr=fopen("C://Users//mobntic//Desktop//generator//request.txt","r");
j=0;
c='b';
char v[20];
while (c!=EOF)
{ 
        c=fgetc(fr); 
        if (c=='\n' || c==EOF)
        {
          v[j]='\0'; 
          int index2=0;
          bool found=false;
          //printf("%s\n",v);
          while (index2<M && found==false)
           {
           // printf("%s\n",hash_table[index2].content);
            if (strcmp_new(hash_table[index2].content, v)==true)
            {
             found=true; 
             //printf ("ok\n");
            }
             else {
                 index2++;
                  }
            }
            if (found==true)
              {
               req[index2]=1;
              } 
            j=0;
        }
        if (c!='\n'){v[j]=c;j++;}
    }    
//printf("\n");
fclose(fr); 
/************for the documents file**********/
f=fopen("C://Users//mobntic//Desktop//generator//documents_1000.txt","r");
//if (f!=NULL) {/*printf("the file is succefully opened\n");*/} 
j=0;
c='a';
char tt[20];
while (c!=EOF)
{ 
        c=fgetc(f); 
        if (c==' ' || c==EOF){
        tt[j]='\0'; 
        int index3=0;
        bool found=false;
        while (index3<M && found==false)
        {
         if (strcmp(hash_table[index3].content, tt)==0)
         {found=true;}
         else {index3++;}
        }
        if (found==true)
        {
        list_doc[i].list_term[index3].occurence++;
        strcpy(list_doc[i].list_term[index3].content, tt);
        } 
         j=0;
        }
        if (c=='#'){list_doc[i].ID_doc=i; i++;}
        if (c!=' ' && c!='\n' && c!='#'){tt[j]=c;j++;}
}    
//printf("\n");
fclose(f); 
}
/*** search the relevant documents according to the given requets using classical method********/
void classical_RI_process()
{
  for (int i=0;i<N;i++)
    {
           int score=0;
      for (int j=0;j<M;j++)
         {
         score=score+(req[j]*list_doc[i].list_term[j].occurence);      
         }   
         relevent_docs1[i]=score;    
    }   
}
void ARM_RI_process(int packet)
{
   for (int i=((N/packets)*(packet-1)); i<((N/packets)*packet);i++)
    {
           int score=0;
      for (int j=0;j<M;j++)
         {
         score=score+(req[j]*list_doc[i].list_term[j].occurence);      
         }   
         relevent_docs2[i]=score;    
    }     
}

/****************ARM prototypes*********************
/**********************copry t in v********/
void copy(int t[], int v[])
{
for (int i=0;i<M; i++)
{
v[i]=t[i]; 
}     
}
/***********trnasformation base documents to transactional base********/
void transformation_transactions()
{
for (int i=0;i<N;i++)
{
    for (int j=0;j<M;j++)
     {
     if (list_doc[i].list_term[j].occurence==0)
     {
      dataset[i].Items[j]=0;
     }        
     else {
          dataset[i].Items[j]=1;
          }
     }
}
}
/*********************create a solution reference Sref******************************************/
void create_Sref(int packet)
{
int nb_items=2;
for (int i=0;i<M;i++)
	{ 
         if ( i>((M/packets)*(packet-1)) && i<((M/packets)*packet) && nb_items!=0)
         {
        int x=rand()%3;
		if (x==1){Sref.solution[i]=0 ;}
		 else {
			if (x==0){Sref.solution[i]=0;}
			else {
				Sref.solution[i]=rand()%3;
				nb_items--;
			      }
		      }
        }
        else {Sref.solution[i]=0;}
		
	}
Sref.cost=fitness1(Sref.solution, packet);
}
/*************************compute the support of the solution s**********/
float support_rule(int s[],int packet)
{
float compt=0;
for (int i=0; i<N; i++)
    {
      bool appartient=true;
		
		int j=((M/packets)*(packet-1));
		while ( j<((M/packets)*packet))
		{
          if (s[j]!=0)
            {
				if (dataset[i].Items[j]==0)
				{
                 appartient=false;
                 }
		     }
		j++;	
		}
		if (appartient==true) {compt++;}
      }
	   compt=(compt/(N/packets));
return compt;
}
/*****************************support antecedent computing*****************************/
float support_antecedent(int s[], int packet)
{
float compt=0;
for (int i=0; i<N; i++)
    {
      bool appartient=true;
		
		int j=((M/packets)*(packet-1));
		while ( j<((M/packets)*packet))
		  {
          if (s[j]==1)
            {
				if (dataset[i].Items[j]==0)
				{
                 appartient=false;
                 }
		     }
		j++;	
		}
		if (appartient==true) {compt++;}
      }
	   compt=(compt/(N/packets));
return compt;
}
/*********************the support of the consequent part*************************/	
float support_consequent(int s[], int packet)
{
float compt=0;
for (int i=0; i<N; i++)
    {
      bool appartient=true;
        int j=((M/packets)*(packet-1));
		while ( j<((M/packets)*packet))
		{
          if (s[j]==2)
            {
				if (dataset[i].Items[j]==0)
				{
                 appartient=false;
                 }
		     }
		j++;	
		}
		if (appartient==true) {compt++;}
      }
	   compt=(compt/(N/packets));
return compt;    
}
/****************************condifence computing**************************/
float confidence(int s[], int packet)
{
	float conf=0;
	conf=support_rule(s, packet)/support_antecedent(s, packet);
return conf;
}
/*********************lift of the rule*********/
float lift (int s[], int packet)
{
 float nb1=confidence(s, packet);
 float nb2=support_consequent(s, packet);
 return(nb1/nb2);  
}
//**************the leverage of the rule*******/
float leverage(int s[], int packet)
{
float nb1=support_rule(s, packet);
float nb2=support_antecedent(s, packet);
float nb3=support_consequent(s, packet);
return (nb1-(nb2*nb3));    
}
////////************the coverage of teh rule*********/
float coverage(int s[], int packet)
{
return support_antecedent(s, packet);      
}
/***********************evaluation of the solution s******/
float fitness1(int s[], int packet)
{
float cost=0; 
float x=alpha*support_rule(s, packet);
float y=Beta*confidence(s, packet);
cost=x+y;
return cost;
}
/***********************************neighborhood computation************************/
bee neighborhood_computation(bee S, int packet)
{
bee s;
int indice=0; 
int neighbor[M], best_neighbor[M];
float current_cost=S.cost;
float best_cost=S.cost;
     copy(S.solution,best_neighbor);

      for (int j=((M/packets)*(packet-1));j<((M/packets)*packet);j++)
        {   
        copy(S.solution,neighbor);	    
        int temp=neighbor[indice];
        neighbor[indice]=neighbor[indice+1];
        neighbor[indice+1]=temp;	
        //neighbor[indice]=0;   
        indice++;
	     //if (indice>=(M/packets)){indice=0;}   
	     current_cost=fitness1(neighbor, packet); //evaluates the solution 
		 if (current_cost>best_cost){copy(neighbor,best_neighbor);best_cost=current_cost;}
		 }
copy(best_neighbor, s.solution);
s.cost=best_cost;
return s;
}
/***********************************Search Area Strategy************************/
void search_area(bee S, int packet)
{
int indice=0; 
int i=0;
     while (i<k)
     {
     copy(S.solution,T_Dance[i].solution);
        for (int l=((M/packets)*(packet-1))+i;l<flip;l++)
        {
           int temp=T_Dance[i].solution[indice+l];
           T_Dance[i].solution[indice+l]=T_Dance[i].solution[indice+1+l];
           T_Dance[i].solution[indice+1+l]=temp;	
        //neighbor[indice]=0;   
        }
        indice++;
         //if (indice>=(M/packets)){indice=0;}   
	     T_Dance[i].cost=fitness1(T_Dance[i].solution, packet); //evaluates the solution 
      i++;
      }
}
/******************return the best dance after the exploration of search region of each bee**********/
int best_dance()
{
	float max=T_Dance[0].cost;
	int indice=0;
	for (int i=1;i<k;i++)	
	{
     	if (T_Dance[i].cost>max)
	     {     
           max=T_Dance[i].cost;
		   indice=i;
         }
	}
return indice;
}
/**********exctract rules for each packet of documents**********/
void ARM_process(int packet)
{ 
bee best; 
int i,j;
            create_Sref(packet);
            copy(Sref.solution, best.solution);
            best.cost=Sref.cost;
            search_area(Sref, packet); 
         for ( i=0; i<IMAX;i++)
	      {
	    int existe=0,l=0,indice=0;			
	        for (j=0;j<k;j++) // neighborhood computation for all the solution in tab
			  		{ 
				     T_Dance[j]=neighborhood_computation(T_Dance[j], packet);
                      }
				    j=best_dance();
					copy(T_Dance[j].solution,Sref.solution);
					Sref.cost=T_Dance[j].cost;
					if (Sref.cost > best.cost)//atte o maximisation
			    { 
					 copy(Sref.solution, best.solution);
					 best.cost=Sref.cost;
			    }
      search_area(Sref, packet);
      //display_solution(best, packet);
          }// Bso ending
}
/**********store the rules of a given packet in rules base from a given index*******/
void stored_rules(int packet)
{ 
  for (int i=0;i<k;i++)
  {
     if ( T_Dance[i].cost!=0)
      {
       copy(T_Dance[i].solution, rules_base[index].rule);
       rules_base[index].packet=packet;
       index++;
      }
  }
}
/*********compute score between request and each packet*******/
int scores(int packet)
{
 int i=0;
 while (rules_base[i].packet!=packet && i<index)
  {
  i++;     
  }   
  //printf ("start's cursor is %d\n", i);
    int sc=0;
    while (rules_base[i].packet==packet && i<index)
     {
            for (int j=0;j<M;j++)
                {
                 if (req[j]==1) 
                     {
                        if (rules_base[i].rule[j]==1 || rules_base[i].rule[j]==2)
                         {        
                          sc++;
                         }
                     }
                 }
            i++;
     }
     //printf ("end's cursor is %d\n", i);
return sc;
}
/********Display functions*******************/

/****************************display_documents***********/
void display_struct()
{
for (int i=0;i<N;i++)
  { 
    for (int j=0;j<M;j++)
     {
     if (list_doc[i].list_term[j].occurence!=0)
     printf ("%s %d  ",list_doc[i].list_term[j].content, list_doc[i].list_term[j].occurence);
     }
     printf ("\n**************************\n");
   }
   
/*for (int j=0;j<M;j++)
    {
     printf ("%s\n",hash_table[j].content);
    }
     printf ("\n**************************\n");
     */
}
/*********************display_transactional database******/
void display_transactions()
{
for (int i=0;i<N;i++)
  { 
    for (int j=0;j<M;j++)
     {
     printf ("%d",dataset[i].Items[j]);
     }
     printf ("\n**************************\n");
   }
}
/**************************display_solution*****************/
void display_solution(bee S, int packet)
{
for (int i=((M/packets)*(packet-1));i<((M/packets)*(packet));i++)
{
    printf("%d ", S.solution[i]);
}
printf ("cost is:%f",S.cost);
printf("\n");
}
/******************display_rules************/
void display_rules()
{
     for (int i=0;i<index;i++)
     {
         for (int j=0;j<M;j++)
          {
          printf("%d ", rules_base[i].rule[j]);
          }
          printf ("packet is %d\n", rules_base[i].packet);
     }
}
/*****************display_request*********/
void display_request()
{
     printf ("user's request is:");
     for (int j=0;j<M;j++)
          {
          printf("%d ", req[j]);
          }
          printf ("\n");
}
/******display relevent documents*******/
void display_relevent_docs()
{
for (int i=0;i<N;i++)
{
printf ("%f***%f\n", relevent_docs1[i], relevent_docs2[i]  );  
}        
}
/***************efficiency***************************/
float efficiency()
{
float s=0;
int nb=0;
for (int i=0;i<N;i++)
{
if (relevent_docs1[i]!=0)
{
nb++;
float d=relevent_docs1[i]-relevent_docs2[i];  
if (d==0){s++;}
}
}
return (s/nb);      
}
