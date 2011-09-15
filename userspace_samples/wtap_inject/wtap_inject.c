/*-
 * Copyright (c) 2010-2011 Monthadar Al Jaberi, TerraNet AB
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce at minimum a disclaimer
 *    similar to the "NO WARRANTY" disclaimer below ("Disclaimer") and any
 *    redistribution must be conditioned upon including a substantially
 *    similar Disclaimer requirement for further binary redistribution.
 *
 * NO WARRANTY
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF NONINFRINGEMENT, MERCHANTIBILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.
 *
 * $FreeBSD$
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>

#include "ieee80211.h"
#include "ieee80211_mesh.h"

#ifdef DEBUG
#define DPRINTF(...) printf(__VA_ARGS__)
#else
#define DPRINTF(...)
#endif

#define MAX_PACKET_SIZE (128)

extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;
extern int optreset;


static int dev = -1;

static void inject_frame(uint8_t *buff, int data_len)
{
	size_t n = write(dev, buff, data_len);
	if(n < 0){
		printf("error writing\n");
		exit(-1);
	}else if(n != data_len){
		printf("incomplete write operation, n=%d, data_len=%d\n", n, data_len);
	}

}

static void usage(){
	printf("usage: chirp_inejct -i path [-p size] [-l num]\n");
	printf("OPTIONS\n");
	printf("\t-h\tThis help message.\n");
	printf("\t-i\tPath to a file describing the different fields in the frame header.\n");
	printf("\t-p\tPayload size. Data will be generated with the pattern 0xEE.\n");
	printf("\t-l\tNumber of times to send the packet. Default 1.\n");
}

static void byte_parse(int bytes, uint8_t *ptr, char *str, char *debug_str){
	char *c[bytes];
	int i;
	if(bytes == 0 || ptr == NULL || str == NULL){
		printf("%s: error cant parse nothing!\n", debug_str);
		exit(-1);
	}
	c[0] = strtok(str, ":"); //first token
	ptr[0] = (uint8_t) strtol(c[0], NULL, 16); //parse hex to decimal
	DPRINTF("parsing %s => parsed: %s", debug_str, c[0]);
	for(i=1; i<bytes; ++i){
		c[i] = strtok(NULL, ":");
		ptr[i] = (uint8_t) strtol(c[i], NULL, 16);
		DPRINTF(":%s", c[i]);
	}
	DPRINTF("\n");
}

void parse_3addr(FILE *file, struct ieee80211_frame *wh){
	char line[255];
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(2, wh->i_fc, line, "frame control");
	else
		goto exit;
	
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(2, wh->i_dur, line, "duration");
	else
		goto exit;
	
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(6, wh->i_addr1, line, "addr1");
	else
		goto exit;
	
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(6, wh->i_addr2, line, "addr2");
	else
		goto exit;

	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(6, wh->i_addr3, line, "addr3");
	else
		goto exit;
	
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(2, wh->i_seq, line, "sequantial control");
	else
		goto exit;

	return;
exit:
	printf("error parsing: %s\n",__func__);
	fclose(file);
	exit(-1);
}


void parse_3addrqos(FILE *file, struct ieee80211_qosframe *wh){
	char line[255];
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(2,wh->i_fc, line, "frame control");
	else
		goto exit;
	
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(2,wh->i_dur, line, "duration");
	else
		goto exit;
	
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(6,wh->i_addr1, line, "addr1");
	else
		goto exit;
	
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(6,wh->i_addr2, line, "addr2");
	else
		goto exit;

	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(6,wh->i_addr3, line, "addr3");
	else
		goto exit;
	
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(2,wh->i_seq, line, "sequantial control");
	else
		goto exit;
	
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(2,wh->i_qos, line, "QoS control");
	else
		goto exit;
	return;
exit:
	printf("error parsing: %s\n",__func__);
	fclose(file);
	exit(-1);
}

void parse_4addr(FILE *file, struct ieee80211_frame_addr4 *wh){
	char line[255];
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(2,wh->i_fc, line, "frame control");
	else
		goto exit;
	
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(2,wh->i_dur, line, "duration");
	else
		goto exit;
	
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(6,wh->i_addr1, line, "addr1");
	else
		goto exit;
	
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(6,wh->i_addr2, line, "addr2");
	else
		goto exit;

	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(6,wh->i_addr3, line, "addr3");
	else
		goto exit;
	
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(2,wh->i_seq, line, "sequantial control");
	else
		goto exit;
	
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(6,wh->i_addr4, line, "addr4");
	else
		goto exit;
	return;
exit:
	printf("error parsing: %s\n",__func__);
	fclose(file);
	exit(-1);
}

void parse_4addrqos(FILE *file, struct ieee80211_qosframe_addr4 *wh){
	char line[255];
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(2,wh->i_fc, line, "frame control");
	else
		goto exit;
	
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(2,wh->i_dur, line, "duration");
	else
		goto exit;
	
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(6,wh->i_addr1, line, "addr1");
	else
		goto exit;
	
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(6,wh->i_addr2, line, "addr2");
	else
		goto exit;

	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(6,wh->i_addr3, line, "addr3");
	else
		goto exit;
	
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(2,wh->i_seq, line, "sequantial control");
	else
		goto exit;
	
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(6,wh->i_addr4, line, "addr4");
	else
		goto exit;
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(2,wh->i_qos, line, "QoS control");
	else
		goto exit;
	return;
exit:
	printf("error parsing: %s\n",__func__);
	fclose(file);
	exit(-1);
}

void parse_mesh_ae00(FILE *file, struct ieee80211_meshcntl *mc){
	char line[255];
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(1,&mc->mc_flags, line, "mesh control flags");
	else
		goto exit;
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(1,&mc->mc_ttl, line, "mesh control TTL");
	else
		goto exit;
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(4,mc->mc_seq, line, "mesh control sequence");
	else
		goto exit;
	return;
exit:
	printf("error parsing: %s\n",__func__);
	fclose(file);
	exit(-1);
}

void parse_mesh_ae01(FILE *file, struct ieee80211_meshcntl_ae01 *mc){
	char line[255];
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(1,&mc->mc_flags, line, "mesh control flags");
	else
		goto exit;
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(1,&mc->mc_ttl, line, "mesh control TTL");
	else
		goto exit;
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(4,mc->mc_seq, line, "mesh control sequence");
	else
		goto exit;
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(6,mc->mc_addr4, line, "mesh control addr4");
	else
		goto exit;
	return;
exit:
	printf("error parsing: %s\n",__func__);
	fclose(file);
	exit(-1);
}

void parse_mesh_ae10(FILE *file, struct ieee80211_meshcntl_ae10 *mc){
	char line[255];
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(1,&mc->mc_flags, line, "mesh control flags");
	else
		goto exit;
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(1,&mc->mc_ttl, line, "mesh control TTL");
	else
		goto exit;
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(4,mc->mc_seq, line, "mesh control sequence");
	else
		goto exit;
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(6,mc->mc_addr4, line, "mesh control addr4");
	else
		goto exit;
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(6,mc->mc_addr5, line, "mesh control addr5");
	else
		goto exit;
	return;
exit:
	printf("error parsing: %s\n",__func__);
	fclose(file);
	exit(-1);
}

void parse_mesh_ae11(FILE *file, struct ieee80211_meshcntl_ae11 *mc){
	char line[255];
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(1,&mc->mc_flags, line, "mesh control flags");
	else
		goto exit;
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(1,&mc->mc_ttl, line, "mesh control TTL");
	else
		goto exit;
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(4,mc->mc_seq, line, "mesh control sequence");
	else
		goto exit;
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(6,mc->mc_addr4, line, "mesh control addr4");
	else
		goto exit;
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(6,mc->mc_addr5, line, "mesh control addr5");
	else
		goto exit;
	if(fgets(line, sizeof(line), file) != NULL)
		byte_parse(6,mc->mc_addr6, line, "mesh control addr6");
	else
		goto exit;
	return;
exit:
	printf("error parsing: %s\n",__func__);
	fclose(file);
	exit(-1);
}

int populate_header(char *path, uint8_t **ptr){
	FILE *file;
	char line[255];
	int hdr_size = 0;
	int mc_size = 0;
	if((file=fopen(path, "r")) == NULL){
		perror(path);
		exit(-1);
	}

	// parse header
	if(fgets(line, sizeof(line), file) != NULL){
		if(strcmp("3addr\n", line) == 0){
			struct ieee80211_frame *wh = (struct ieee80211_frame *)*ptr;
			hdr_size = sizeof(struct ieee80211_frame);
			DPRINTF("3addr\n");
			parse_3addr(file, wh);
		}else if(strcmp("3addrqos\n", line) == 0){
			struct ieee80211_qosframe *wh = (struct ieee80211_qosframe *)*ptr;
			hdr_size = sizeof(struct ieee80211_qosframe);
			DPRINTF("3addrqos\n");
			parse_3addrqos(file, wh);
		}else if(strcmp("4addr\n", line) == 0){
			struct ieee80211_frame_addr4 *wh = (struct ieee80211_frame_addr4 *)*ptr;
			hdr_size = sizeof(struct ieee80211_frame_addr4);
			DPRINTF("4addr\n");
			parse_4addr(file, wh);
		}else if(strcmp("4addrqos\n", line) == 0){
			struct ieee80211_qosframe_addr4 *wh = (struct ieee80211_qosframe_addr4 *)*ptr;
			hdr_size = sizeof(struct ieee80211_qosframe_addr4);
			DPRINTF("4addrqos\n");
			parse_4addrqos(file, wh);
		}
		*ptr += hdr_size;
	}else
		goto exit;
	
	// check if we are to parse mesh control structures
	if(fgets(line, sizeof(line), file) != NULL){
		if(strcmp("mesh_ae00\n", line) == 0){
			struct ieee80211_meshcntl *mc = (struct ieee80211_meshcntl *)*ptr;
			mc_size = sizeof(struct ieee80211_meshcntl);
			DPRINTF("mesh_ae00\n");
			parse_mesh_ae00(file, mc);
		}else if(strcmp("mesh_ae01\n", line) == 0){
			struct ieee80211_meshcntl_ae01 *mc = (struct ieee80211_meshcntl_ae01 *)*ptr;
			mc_size = sizeof(struct ieee80211_meshcntl_ae01);
			DPRINTF("mesh_ae01\n");
			parse_mesh_ae01(file, mc);
		}else if(strcmp("mesh_ae10\n", line) == 0){
			struct ieee80211_meshcntl_ae10 *mc = (struct ieee80211_meshcntl_ae10 *)*ptr;
			mc_size = sizeof(struct ieee80211_meshcntl_ae10);
			DPRINTF("mesh_ae10\n");
			parse_mesh_ae10(file, mc);
		}else if(strcmp("mesh_ae11\n", line) == 0){
			struct ieee80211_meshcntl_ae11 *mc = (struct ieee80211_meshcntl_ae11 *)*ptr;
			mc_size = sizeof(struct ieee80211_meshcntl_ae11);
			DPRINTF("mesh_ae11\n");
			parse_mesh_ae11(file, mc);
		}
		*ptr += mc_size;
	}
	
	DPRINTF("done parsing\n");
	fflush(file);
	fclose(file);
	return hdr_size+mc_size;
exit:
	printf("error parsing file\n");
	fclose(file);
	exit(-1);
}

int main(int argc, char **argv)
{
	uint8_t *buff = (uint8_t *) calloc(1, MAX_PACKET_SIZE);
	int data_len = 0; //total length of packet
	int payload = 0; //length of payload data after frame struct (and ev. mesh control struct)
	int ch;
	uint8_t *crr_ptr = buff; // current pointers position
	int loop = 1; // number of times we call ioctl
	int file_loaded = 0;
	int i;
	char path[255];
	
	while ((ch = getopt(argc, argv, "hi:p:l:")) != -1) {
		switch (ch) {
		case 'i':
			sscanf(optarg, "%s", path);
			file_loaded = 1;
			break;
		case 'p':
			sscanf(optarg, "%d", &payload);
			data_len += payload;
			break;
		case 'l':
			sscanf(optarg, "%d", &loop);
			break;
		case 'h':
		case '?':
		default:
			usage();
			return -1;
		}
	}
	argc -= optind;
	argv += optind;
	
	if(!file_loaded){
		printf("Error, you need to load a file describing the different fields in the frame header\n");
		return -1;
	}
	
	dev = open("/dev/wtap0", O_WRONLY);
	if(dev < 0){
		printf("error opening wtapnode0 cdev\n");
		return -1;
	}
	
	data_len += populate_header(path, &crr_ptr);
	
	if(data_len > MAX_PACKET_SIZE) {
		printf("Error: data total length=%d > MAX_PACKET_SIZE=%d\n", data_len, MAX_PACKET_SIZE);
		return -1;			
	}
	
	//fill payload
	for(i=0; i<payload; ++i){
		*crr_ptr = 238; // 0xEE
		++crr_ptr;
	}
	for(i=0; i<loop; ++i){
		printf("calling IOCTL packet number %d\n", i);
		inject_frame(buff, data_len);
	}
	return 0;
}
