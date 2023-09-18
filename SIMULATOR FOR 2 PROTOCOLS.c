
/* header files for the program */

#include<stdio.h>
#include<math.h>
#include<string.h>
#include<stdlib.h>
#include "lcgrand.h"	
#define Q_LIMIT 10000000	
#define BUSY 1
#define IDLE 0

/* global declarations for the program */

int next_event_type,num_delays_required,num_events,server_status,state_position;
int num_custs_delayed1,num_in_q1,num1=0,num11 = 0,packet_tracker1=0;
int num_custs_delayed2,num_in_q2,num2=0,num22 = 0,packet_tracker2=0;


float avg_area_in_q1,mean_interarrival1,mean_service1,time_arrival1[Q_LIMIT + 1],time_dep1[Q_LIMIT + 1],total_of_delays1,mean_state_change1;
float avg_area_in_q2,mean_interarrival2,mean_service2,time_arrival2[Q_LIMIT + 1],time_dep2[Q_LIMIT + 1],total_of_delays2,mean_state_change2;
float area_server_status,sim_time,time_last_event,time_next_event[6],delay_cust[Q_LIMIT + 1];


FILE *errorFile,*outfile,*infile,*individualStatistics;
FILE *arrival1,*depart1,*delay1,*custsInQ1;
FILE *arrival2,*depart2,*delay2,*custsInQ2;


/* function declarations for the program */

void initialize(void);
void timing(void);
void calculationFunction(int count);
void update_time_avg_stats(void);
float expon(float mean);
void showfile(char filename[100]);

void arrive1Fun(int count_custs);
void depart1Fun(int count);
void arrive2Fun(int count_custs);
void depart2Fun(int count);
void stateChange12(void);
void stateChange21(void);
void state1Function(void);
void state2Function(void);

/* main controller */

int main(){
	
	int sim_count = 1, itteration_sim,i,j;
	num_events = 6;
	
	
	// accept numer of time to run the simulator
	printf("\nHow many number of times ? \t");
	scanf("%d",&itteration_sim);
	// read input file content and store in variables
	infile = fopen("input.txt","r");
	fscanf(infile,"%f %f %f  %f %f  %f  %d", &mean_interarrival1,&mean_interarrival2,&mean_service1,&mean_service2,&mean_state_change1,&mean_state_change2,&num_delays_required);
	
	//OPEN ALL FILES TO STORE CONTENTS
	errorFile = fopen("error.txt","w");
	individualStatistics = fopen("individual.txt","w");
	
	arrival1 = fopen("arrival1.txt","w");
	arrival2 = fopen("arrival2.txt","w");
	
	depart1 = fopen("depart1.txt","w");
	depart2 = fopen("depart2.txt","w");
	
	delay1 = fopen("delay1.txt","w");
	delay2 = fopen("delay2.txt","w");
	
	custsInQ1 = fopen("custsInQ1.txt","w");
	custsInQ2 = fopen("custsInQ2.txt","w");
	
	//initialize global variable	
	initialize();
	
	//run loop untill the required number of customers are served
	while((num_custs_delayed1+num_custs_delayed2) < num_delays_required)
	{	
		// update timing for next events exponentially and decide next_even_type
		timing();		
		update_time_avg_stats();
			
		switch(state_position)
		{		
			case 1:
				state1Function();
				break;
			case 2:
				state2Function();
				break;	
		}
		
	}
	
	
	//optional statistics to write to respective files, just to have in hand data
	fprintf(custsInQ1,"\nTotal # custs in Q1 throughout simulation  %f\n",avg_area_in_q1);
	fprintf(custsInQ1,"Total # time  %f\n",sim_time);
	fprintf(custsInQ1,"(Avg # custs in Q1)/T  =  %f\n",avg_area_in_q1/sim_time);
	fclose(custsInQ1);
	fprintf(individualStatistics,"\n\t AVERAGE NUMBER OF CUSTOMERS IN Q1\t= \t%f\n",avg_area_in_q1/sim_time);	
		
	fprintf(custsInQ2,"\nTotal # custs in Q2 throughout simulation  %f\n",avg_area_in_q2);
	fprintf(custsInQ2,"Total # time  %f\n",sim_time);
	fprintf(custsInQ2,"(Avg # custs in Q2)/T  =  %f\n",avg_area_in_q2/sim_time);
	fclose(custsInQ2);
	fprintf(individualStatistics,"\n\t AVERAGE NUMBER OF CUSTOMERS IN Q2\t= \t%f\n",avg_area_in_q2/sim_time);
	
	
	fprintf(delay1,"\nTotal # delay1  %f\n",total_of_delays1);
	fprintf(delay1,"(Avg # delay1)/N  =  %f\n",total_of_delays1/num_custs_delayed1);
	fclose(delay1);
	fprintf(individualStatistics,"\n\t AVERAGE DELAY IN QUEUE 1\t =  \t%f\n",total_of_delays1/num_custs_delayed1);
	
	fprintf(delay2,"\nTotal # delay2  %f\n",total_of_delays2);
	fprintf(delay2,"(Avg # delay2)/N  =  %f\n",total_of_delays2/num_custs_delayed2);
	fclose(delay2);
	fprintf(individualStatistics,"\n\t AVERAGE DELAY IN QUEUE 2\t =  \t%f\n",total_of_delays2/num_custs_delayed2);
	
	//close these output files for each opeation
	fclose(arrival1);
	fclose(arrival2);
	fclose(depart1);
	fclose(depart2);
	fclose(individualStatistics);
		
	while(sim_count<=itteration_sim)
	{
		//output file to store the statistics of the simulation
		outfile = fopen("output.txt","w");
			
		//perform the calculations multiple times
		calculationFunction(sim_count);
		
		fclose(outfile);	
		
		printf("\n\t\t   Round   \t\t %d",sim_count);
		printf("\n\t ........................................");
			
		sim_count++;
	}
	
	
		
	outfile = fopen("output.txt","a");
	fprintf(outfile,"\n\tTotal customers served  =  %d\n",itteration_sim*num_delays_required);
	fclose(outfile);	


	// optional part: show contents of different files
	// these files are created and displayed for testing and
	// verification purpose.
		
	printf("\n Arrival1 of Custs: \n\n");
	showfile("arrival1.txt");
	printf("\n\n Departure from queue 2 of Custs\n\n");
	showfile("depart2.txt");
	printf("\n\n # delay in queue 1\n\n");
	showfile("delay1.txt");
		
	printf("\n Arrival2 of Custs\n\n");
	showfile("arrival2.txt");
	printf("\n\n Departure from queue 1 of Custs \n\n");
	showfile("depart1.txt");
	printf("\n\n # delay in queue 2\n\n");
	showfile("delay2.txt");

	printf("\n\n # custs * time in Q1\n\n");
	showfile("custsInQ1.txt");
	printf("\n\n # custs * time in Q2\n\n");
	showfile("custsInQ2.txt");

	// the show stastics section
	printf("\n\n Simulation OutPut file content \n\n");
	// entire simulation statistics	
	showfile("output.txt");
	
	fclose(infile);
	
	return 0;
}

void state1Function()
{	
	switch(next_event_type)
	{			
		case 1:
			arrive1Fun(num1);
			num1++;
			break;
		case 2:
			state_position = 2;
			break;
		
		case 3:
			depart1Fun(num11);
			num11++;
			break;
		case 4:
			state_position = 2;
			break;
		
		case 5:
			stateChange12();
			state_position = 2;
			break;
		case 6:
			state_position = 2;
			break;
				
	}
	
}

void state2Function()
{	
	switch(next_event_type)
	{
		
		case 1:
			state_position = 1;
			break;
		case 2:
			arrive2Fun(num2);
			num2++;
			break;
		case 3:
			state_position = 1;
			break;
		case 4:
			depart2Fun(num22);
			num22++;
			break;
		case 5:
			state_position = 1;
			break;
		case 6:
			stateChange21();
			state_position = 1;
			break;		
	}
	
}

void initialize(){

	state_position = 1;
	sim_time= 0.0;
	server_status = IDLE;
	area_server_status = 0.0;
	time_last_event = 0.0;
	
	num_in_q1 = 0;
	num_in_q2 = 0;
	
	num_custs_delayed1 = 0;
	num_custs_delayed2 = 0;
			
	total_of_delays1 = 0.0;
	total_of_delays2 = 0.0;
	
	avg_area_in_q1 = 0.0;
	avg_area_in_q2 = 0.0;
	
	//since there are no customers , departure cant happen at the begening only switching and arrivals are possible
	// therefore departures are given long time
	time_next_event[1] = sim_time + expon(mean_interarrival1);
	time_next_event[2] = sim_time + expon(mean_interarrival2);
	time_next_event[3] = 1.0e+30;
	time_next_event[4] = 1.0e+31;
	time_next_event[5] = sim_time + expon(mean_state_change1);
	time_next_event[6] = sim_time + expon(mean_state_change2);
 }
 
 
 void timing(){

 	int i;
 	float min_time_next_event = 1.0e+29;
 	
 	next_event_type = 0;
 	
 	// decide which event to happen next.
 	// this process is a random process using exponential variable
 	for(i=1;i<= num_events;++i){
 		
 		if(time_next_event[i] < min_time_next_event){
 			min_time_next_event = time_next_event[i];
 			next_event_type = i;	
		 }
 		
	 }
 	
 	if(next_event_type == 0){
 		
 		fprintf(errorFile,"\n event list empty at time %f",sim_time);
 		exit(1);
	 }
	 
	 sim_time = min_time_next_event;
	 
 }
 
 void update_time_avg_stats(){

 	float time_since_last_event;
	
	//time since last event 	
 	time_since_last_event = sim_time - time_last_event;
 	time_last_event = sim_time;
 	
 	// optional for testing purpose
 	// here we can find how many customers are in Q1 and Q2 at time T
	fprintf(custsInQ1,"%d    *    %f    =   %f\n",num_in_q1,time_since_last_event,num_in_q1*time_since_last_event);
	fprintf(custsInQ2,"%d    *    %f    =   %f\n",num_in_q2,time_since_last_event,num_in_q2*time_since_last_event); 
 	
 	// calculate number of customers residing in Q1 & Q2
 	// it is tak over continous time
 	avg_area_in_q1 += num_in_q1 * time_since_last_event;
 	avg_area_in_q2 += num_in_q2*time_since_last_event;
 	
 	// calculate the utilization of the sever
 	// it can be either IDLE or BUSY at time T
 	// it is taken over contineous time
	 area_server_status += server_status*time_since_last_event;
 }
 
 

// arrival of type 1 is deployed to Q2 to overrcome the interoperability problem
 void arrive1Fun(int count_custs){
 	
 	float delay;
	
 	time_next_event[1] = sim_time + expon(mean_interarrival1);
 	
 	if(server_status == BUSY)
	 {
 		++num_in_q2;
		
		// this condition can never happen because our system is using infinite queue capacity
 		if(num_in_q2 > Q_LIMIT)
		 {
 			fprintf(errorFile,"\n overflow of the array time_arrival at time %f",sim_time);	
		 }
	 }
	 
	 else
	 {	
	 	
	 	//since server is not BUSY, the customer has not to wait in Q
	 	//it is immediatly departed from Q
	 	delay = 0.0;
	 	//write value to file for testing purpose
	    fprintf(delay2,"\n delay -> %f",delay);
	    packet_tracker2++;
	 	total_of_delays2 += delay;
	 	
	 	// mark departure from queue time in a separate array
	 	time_dep2[num_custs_delayed2] = sim_time;
		//write value to file for testing purpose
	  	fprintf(depart2,"%f\n",time_dep2[num_custs_delayed2]);
		
		
		// now one customer has been given to server, the server is BUSy now
		// after AVG_SERVICE amount of time the packet will be departed from the system also	
	 	++num_custs_delayed2;
	 	server_status = BUSY;
	 	
	 	
	 	//calculate next arrival times for each event
	 	time_next_event[3] = sim_time + expon(mean_service1);
	 	time_next_event[4] = sim_time + expon(mean_service2);
	 	time_next_event[5] = sim_time + expon(mean_state_change1);
	 	time_next_event[2] = sim_time + expon(mean_interarrival2);
	 	time_next_event[6] = sim_time + expon(mean_state_change2);
	 }
	 
	 // put arrival time for customer at this exact time
	 time_arrival1[count_custs] = sim_time;
	 fprintf(arrival1,"%f\n",time_arrival1[count_custs]);
	
 }
 
 // arrival of type 2 is deployed to Q1 to overrcome the interoperability problem
  void arrive2Fun(int count_custs){
 	
 	float delay;
 	
 	time_next_event[2] = sim_time + expon(mean_interarrival2);
 	
 	if(server_status == BUSY)
	 {
 		++num_in_q1;
	
 		if(num_in_q1 > Q_LIMIT)
		 {		
 			fprintf(outfile,"\n overflow of the array time_arrival at time %f",sim_time);
		 }
	 }else
	 {	
	 	//since server is not BUSY, the customer has not to wait in Q
	 	//it is immediatly departed from Q
	 	delay = 0.0;
	 	//write value to file for testing purpose
	 	fprintf(delay1,"\n delay -> %f",delay);
	 	packet_tracker1++;
	 	total_of_delays1 += delay;
		
		// store depart from q time
	 	time_dep1[num_custs_delayed1] = sim_time;
	    fprintf(depart1,"%f\n",time_dep1[num_custs_delayed1]);
	 	
	 	// customer given to server , server is BUSY now serving the customer
	 	++num_custs_delayed1;
	 	server_status = BUSY;
	 	
	 	//calculate next arrival times for each event
	 	time_next_event[3] = sim_time + expon(mean_service1);
	 	time_next_event[4] = sim_time + expon(mean_service2);
	 	time_next_event[5] = sim_time + expon(mean_state_change1);
	 	time_next_event[1] = sim_time + expon(mean_interarrival1);
	 	time_next_event[6] = sim_time + expon(mean_state_change2);
	 }
	 
	// store arrival times for arrival 2 customers
	time_arrival2[count_custs] = sim_time;
	fprintf(arrival2,"%f\n",time_arrival2[count_custs]); 
	
 }
 
 
 // departure 1 event is happening from Q2
 // because of the interoperability case.
 // departure from system is almost the same, we need to add avg_service time to the avg_delay in Q value
 void depart1Fun(int count_custs){
 	
 	int i;
 	float delay;
 	
 	if(num_in_q2 == 0){

 		server_status = IDLE;
 		time_next_event[3] = 1.0e+30; 		
	 }
	 else{
	 	--num_in_q2;
	 	
	 	//calculate delay of current customer
	 	delay = sim_time - time_arrival1[packet_tracker2];
	 	fprintf(delay2,"\n delay -> %f",delay);
	 	packet_tracker2++;
	 	total_of_delays2 += delay;
	 	
	 	time_dep2[num_custs_delayed2] = sim_time;
		fprintf(depart2,"%f\n",time_dep2[num_custs_delayed2]);
		
	 	++num_custs_delayed2;
	
	 	time_next_event[3] = sim_time + expon(mean_service1);

		// shift queue content one step further, since one customer is pushed from queue to server
	 	for(i=0; i<num_in_q2; i++){	 		
	 		time_arrival1[i] = time_arrival1[++i];
		 }
	 	
	 	
	 }
	 
 }
 
  // departure 2 event is happening from Q1
 // because of the interoperability case.
 void depart2Fun(int count_custs){
 	
 	int i;
 	float delay;
 	
 	if(num_in_q1 == 0){
	 	
 		server_status = IDLE;
 		time_next_event[4] = 1.0e+30;	
	 }
	 else{
	 	
	 	--num_in_q1;
	 	
	 	// compute delay for current customer
	 	delay = sim_time - time_arrival2[packet_tracker1];
	 	fprintf(delay1,"\n delay -> %f",delay);
	 	total_of_delays1 += delay;
	 	packet_tracker1++;
	 	
	 	time_dep1[num_custs_delayed1] = sim_time;
		fprintf(depart1,"%f\n",time_dep1[num_custs_delayed1]);
		
	 	++num_custs_delayed1;
	 	time_next_event[4] = sim_time + expon(mean_service2);
	 	
		// shift queue content one step
	 	for(i=0; i< num_in_q1; i++){
	 		
	 		time_arrival2[i] = time_arrival2[++i];
		 }
	 }
 }
 
 
void stateChange12(){
	
	//decide when stateChange from state_one to State_two 
	//should happen again, exponentially.
	time_next_event[5] = sim_time + expon(mean_state_change1);
}

void stateChange21(){

	//decide when stateChange from state_two to State_one 
	//should happen again, exponentially.		
	time_next_event[6] = sim_time + expon(mean_state_change2);
}

// calculate randon probabalities using exponential distribution
float expon(float mean){
 	
 	return -mean*log(lcgrand(1));
 }
 
 
 // show file contents
 void showfile(char filename[100]){
 	char str[1000];
 	char* pend;
 	
 	FILE *file;
 	int i =0,j=0;
 	float value;
 	file = fopen(filename,"r");
 	
 	while( fgets (str, 100, file)!=NULL){

		printf("%s",str);
		
	}
	
	fclose(file);
 }
 
 
 //actual statistic calculations
 void calculationFunction(int count){
	
	fprintf(outfile,"\n\tSIMULATION REPORT Round           \t\t%d",count);
	fprintf(outfile,"\n\tMEAN INTERARRIVAL TYPE 1          \t\t%f", mean_interarrival1);
	fprintf(outfile,"\n\tMEAN INTERARRIVAL TYPE 2          \t\t%f", mean_interarrival2);
	fprintf(outfile,"\n\tMEAN SERVICE TIME TYPE 1          \t\t%f",mean_service1);
	fprintf(outfile,"\n\tMEAN SERVICE TIME TYPE 2          \t\t%f",mean_service2);
	fprintf(outfile,"\n\tMEAN SWITCHING OF TYPE 1          \t\t%f",mean_state_change1);
	fprintf(outfile,"\n\tMEAN SWITCHING OF TYPE 2          \t\t%f",mean_state_change2);
	
	fprintf(outfile,"\n\n\t **** COMIBE QUEUES STATISTICS S ****\n");
	
	fprintf(outfile,"\n\tAVERAGE DELAY IN ALL QUEUES       \t\t%f",(total_of_delays1 + total_of_delays2)/(num_custs_delayed1+num_custs_delayed2));
	fprintf(outfile,"\n\tAVERAGE DELAY IN SYSTEM           \t\t%f",((total_of_delays1+total_of_delays2)/(num_custs_delayed1+num_custs_delayed2))+((mean_service1+mean_service2)/2));
 	fprintf(outfile,"\n\tAVERAGE CUSTOMERS IN ALL QUEUES   \t\t%f",(avg_area_in_q1+avg_area_in_q2)/sim_time);
 	fprintf(outfile,"\n\tSERVER UTILIZATION (PERCENT)      \t\t%f%%",(area_server_status/sim_time)*100);
 	fprintf(outfile,"\n\tTOTAL SIMULATION TIME             \t\t%f",sim_time);
 	
 	fprintf(outfile,"\n\n\t **** INDIVIDUAL QUEUE STATISTICS S ****\n");
 	
	fprintf(outfile,"\n\t AVERAGE DELAY IN QUEUE 1\t =  \t\t%f\n",total_of_delays1/num_custs_delayed1);
	fprintf(outfile,"\n\t AVERAGE DELAY IN QUEUE 2\t =  \t\t%f\n",total_of_delays2/num_custs_delayed2);
	fprintf(outfile,"\n\t AVERAGE NUMBER OF CUSTOMERS IN Q1\t= \t%f\n",avg_area_in_q1/sim_time);
 	fprintf(outfile,"\n\t AVERAGE NUMBER OF CUSTOMERS IN Q2\t= \t%f\n",avg_area_in_q2/sim_time);
 }

 
