/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file regsrv.cc
 * @maintainer Yury Oleynik <oleynik@in.tum.de>
 * @status alpha
 *
 * This file implements registry service class
 */
#include <config.h>

#include <cstdio>
#include <iostream>
#include <fstream>
#include <stdlib.h>


#include "regsrv.h"
#include "regsrv_client.h"
#include "regsrv_sockets.h"
#include "scorep_oa_registry_protocol.h"
#include "stringutil.h"


using namespace std;



typedef struct Measurement
{
    int     rank;       ///<MPI process
    int     thread;     ///<thread
    int    	file;     ///<region fileId
    int     rfl;        ///<region first line number
    int     regionType; ///<regionType, e.g., loop, subroutine
    int     samples;    ///<number of measurements
    int     metric;     ///<metric, e.g., execution time
    int     ignore;     ///<number of measurements that could not be performed due to shortage of counters
    double  fpVal;      ///<value as floating point number
    int64_t intVal;     ///<value as integer number
} MeasurementType;

#define MAX_REGION_NAME_LENGTH                          20
#define MAX_FILE_NAME_LENGTH                            150

typedef struct SCOREP_OA_CallPathRegionDef_struct
{
    unsigned long int region_id;
    char     name[ MAX_REGION_NAME_LENGTH ];
    char     file[ MAX_FILE_NAME_LENGTH ];
    unsigned int rfl;
    unsigned int rel;
    unsigned int adapter_type;
}SCOREP_OA_CallPathRegionDef;

typedef struct SCOREP_OA_StaticProfileMeasurement_struct
{
	unsigned long int  measurement_id;
	unsigned int rank;
	unsigned int thread;
	unsigned long int region_id;
	unsigned long int samples;
	unsigned long int metric_id;
	unsigned long int int_val;
}SCOREP_OA_StaticProfileMeasurement;

int RegServ::open( int port )
{
	char hostname[ 100 ];
	gethostname( hostname, 100 );
	printf("Registry service: starting server on %s:%d...",hostname,port);

    server_socket_ = scorep_oa_sockets_server_startup_retry( &port, 1, 1 );
    if ( server_socket_ < 0 )
    {
    	fprintf(stderr,"Selected regisrty port is busy");
    	return -1;
    }
    port_=port;
    printf(" OK\n");
    return server_socket_;
}

void RegServ::close_connection()
{
	close( connection_socket_ );
	printf("Registry service: connection closed!\n");
}

void RegServ::close_server()
{
	close( server_socket_ );
	printf("Registry service: server closed!\n");
}

int RegServ::blocking_accept( )
{
	printf("Registry service: accepting connection...");
    fflush(stdout);
    if (server_socket_<0){
    	fprintf(stderr,"Server is not started!");
    	return -1;
    }
    	
    
    connection_socket_ = scorep_oa_sockets_server_accept_client( server_socket_ );
    
    if( connection_socket_ < 0)
    {
    	fprintf(stderr,"Accepting connection failed!");
    	return -1;
    }
    const int bufsize=400;
    char welcome[bufsize];
  
    sprintf( welcome, MSG_WELCOME, get_hostname().c_str(),
	   get_port(), "---", "---" );
    scorep_oa_sockets_write_line(connection_socket_, welcome);
    
    printf(" OK\n");

    return connection_socket_;
}

int RegServ::execute_test(const char* scenario_file)
{

	printf("Registry service: starting test...\n");
	std::map< int , RegEntry* >::iterator it;
	printf("Registry service: connecting to monitoring processes...");
	for( it=reg_data_.begin(); it!=reg_data_.end(); it++ )
	{

		int sock=scorep_oa_sockets_client_connect_retry(it->second->node.c_str(),it->second->port,1);
		if (sock<0)
		{
				fprintf(stderr,"\nRegistry service: test failed!!! could not connect to the monitoring process at %s:%d\n",
							it->second->node.c_str(),
							it->second->port);
				exit(1);
		}

		it->second->test_comm_sock=sock;
	}
	printf("OK\n");

	//open scenario file
	ifstream f (scenario_file);
	if (!f.is_open()){
		fprintf(stderr,"Registry service: Opening test scenario file failed!\n");
		exit(1);
	}

	string  request;
	int     pos;
	string  delim   = "|"; //delimiter for the request lines in scenario file
	string  none    = "-"; //no confirmation from OA
	string  data     = "data"; //receive data
	string  wait	= "<wait>"; //wait for input from OA
	string  buffer, response, recv_data;

	while (1)
	//loop over lines in the scenario file
	{
		// read one line from the scenario file into buffer
		getline(f, request);
		if (f.eof())
				break;

		pos = get_token(request, 0, delim, buffer);
		pos = get_token(request, pos, delim, response);
		pos = get_token(request, pos, delim, recv_data);

		for( it=reg_data_.begin(); it!=reg_data_.end(); it++ )
		/// loop over registered monitoring processes
		{
			if(buffer == wait)
				printf("Registry service: waiting for message...\n");
			else
			{
				printf("Registry service: sending command to process %i: %s\n", it->second->pid, buffer.c_str());
				scorep_oa_sockets_write_line( it->second->test_comm_sock, (buffer+"\n").c_str() );
			}
			
			if (response != none){
				int maxlen = MAX_MESSAGE_SIZE;
				char buf[ maxlen ];
				buf[ 0 ] = 0;
				bzero( buf, maxlen );
				int length;
				while ( ( length = scorep_oa_sockets_read_line( it->second->test_comm_sock, buf, maxlen ) ) == 0 )
				{
				}
				printf("Registry service: received from process %i: %s\n", it->second->pid, buf);
				if (strcmp(response.c_str(), buf)!=0){
						fprintf(stderr,"Registry service: Wrong answer. Expected: %s.\n", response.c_str());
						exit(1);
				}
			}

			if (recv_data == data)
			{
				int number;
				int nr = scorep_oa_sockets_blockread( it->second->test_comm_sock,
														(char*)(&number), 
														sizeof(int));
				if(nr!=sizeof(int))
				{
					fprintf(stderr,"Registry service: Couldn't receive number of records\n");
					exit(1);
				}
				
				printf("Registry service: Expecting Merged Region Definitions from process %i, size: %d entries\n", 
						it->second->pid, 
						number);
				
				if (number > 0)
				{
					
					SCOREP_OA_CallPathRegionDef* recv_buffer = (SCOREP_OA_CallPathRegionDef *) 
															calloc( number, sizeof(SCOREP_OA_CallPathRegionDef) );
					if (!recv_buffer)
					{
						fprintf(stderr,"Registry service: Error allocating recv_buffer\n");
						exit(1);
					}
					nr=scorep_oa_sockets_blockread( it->second->test_comm_sock,
													(char*)(recv_buffer), 
													number*sizeof(SCOREP_OA_CallPathRegionDef));
													
					if(nr!=number*sizeof(SCOREP_OA_CallPathRegionDef))
					{
						fprintf(stderr,"Registry service: Couldn't receive measurements buffer\n");
						exit(1);
					}
					printf("Registry service: Got Static Regions Definitions from process %i:\n", it->second->pid);
					int i;
					for ( i = 0; i < number; i++ )
					{
						printf("record %d: \t|region_id=%ld \t| name=%s \t| file=%s \t| rfl=%d \t| rel=%d \t| adapter_type=%d\t|\n",
												i,
												recv_buffer[ i ].region_id,
												recv_buffer[ i ].name,
												recv_buffer[ i ].file,
												recv_buffer[ i ].rfl,
												recv_buffer[ i ].rel,
												recv_buffer[ i ].adapter_type
												);
					}	
					free(recv_buffer);
				
				}
				
				int maxlen = MAX_MESSAGE_SIZE;
				char buf[ maxlen ];
				buf[ 0 ] = 0;
				bzero( buf, maxlen );
				int length;
				while ( ( length = scorep_oa_sockets_read_line( it->second->test_comm_sock, buf, maxlen ) ) == 0 )
				{
				}
				printf("Registry service: received from process %i: %s\n", it->second->pid, buf);
				
				number=0;
				nr = scorep_oa_sockets_blockread( it->second->test_comm_sock,
														(char*)(&number), 
														sizeof(int));
				if(nr!=sizeof(int))
				{
					fprintf(stderr,"Registry service: Couldn't receive number of records\n");
					exit(1);
				}
				
				printf("Registry service: Expecting Static Profile from process %i, size: %d entries\n", 
						it->second->pid, 
						number);
				
				if (number > 0)
				{
				
					SCOREP_OA_StaticProfileMeasurement* recv_buffer = (SCOREP_OA_StaticProfileMeasurement *) 
															calloc( number, sizeof(SCOREP_OA_StaticProfileMeasurement) );
					if (!recv_buffer)
					{
						fprintf(stderr,"Registry service: Error allocating recv_buffer\n");
						exit(1);
					}
					nr=scorep_oa_sockets_blockread( it->second->test_comm_sock,
													(char*)(recv_buffer), 
													number*sizeof(SCOREP_OA_StaticProfileMeasurement));
													
					if(nr!=number*sizeof(SCOREP_OA_StaticProfileMeasurement))
					{
						fprintf(stderr,"Registry service: Couldn't receive measurements buffer\n");
						exit(1);
					}
					
					printf("Registry service: Got Static Profile from process %i:\n", it->second->pid);
					int i;
					for ( i = 0; i < number; i++ )
					{
						printf("record %d: \t|meas_id=%ld \t| rank=%d \t| thread=%d \t| region_id=%ld \t| samples=%ld \t| counter=%ld\t| int_val=%ld \t|\n",
												i,
												recv_buffer[ i ].measurement_id,
												recv_buffer[ i ].rank,
												recv_buffer[ i ].thread,
												recv_buffer[ i ].region_id,
												recv_buffer[ i ].samples,
												recv_buffer[ i ].metric_id,
												recv_buffer[ i ].int_val
												);
					}
					
					free(recv_buffer);
				}
			}
		}
	}

	f.close();
	
	//for( it=reg_data_.begin(); it!=reg_data_.end(); it++ )
	//{
	//		close(it->second->test_comm_sock);
	//}
	printf("Registry service: test successfully completed!\n");
	exit(0);
}


void usage( int argc, char* argv[] )
{
  fprintf(stderr, "Usage: %s <portnumber> [test=<filename>]\n", argv[0]);
}

int main( int argc, char* argv[] ){
	
	
	int do_test=0;
	int port=31337;
	int     pos;
	string  delim   = "=";
	string  test_arg, scenario_file;
	
	if( argc>1 ) port=atoi(argv[1]);
	if( argc>2 )
	{
		pos = get_token(argv[2], 0, delim, test_arg);
		pos = get_token(argv[2], pos, delim, scenario_file);
	
		if( strcasecmp(test_arg.c_str(), "test") )
		{
			fprintf(stderr,"Wrong argument %s\n", argv[2] );
			usage(argc, argv);
			exit(1);
		}
		
		if(scenario_file.size()==0)
		{
			fprintf(stderr,"No scenario file provided\n", argv[2] );
			usage(argc, argv);
			exit(1);
		}	
		do_test=1;		
	}
	
	RegServ regsrv;
	
	printf("Registry service: started\n");
	if(do_test)
		printf("Registry service: test requested, using scenario file: %s\n",scenario_file.c_str());
	
	if(regsrv.open(port)<0)
	{
		fprintf(stderr,"Error starting server on %d\n", port );
		usage(argc, argv);
		exit(1);
	}
	
	
	while (1)
	{
		regsrv.blocking_accept();
		RegServClient *client = new RegServClient(&regsrv);
		int mode=REGSRV_MODE_LISTEN;
		while (mode==REGSRV_MODE_LISTEN)
			mode=client->receive_and_handle_request();	
		regsrv.close_connection();
		delete client;
		if(mode==REGSRV_MODE_TEST || do_test==1)
			regsrv.execute_test(scenario_file.c_str());
	}
	regsrv.close_server();
	printf("Registry service: finished");
  return 0;
}
