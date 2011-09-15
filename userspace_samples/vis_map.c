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
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "../src/plugins/visibility_ioctl.h"

static int dev = -1;

static void toggle_medium(int op)
{
    if(ioctl(dev, VISIOCTLOPEN, &op) < 0){
	printf("error opening/closing medium\n");
    }
}

static void link_op(struct link *l)
{
    if(ioctl(dev, VISIOCTLLINK, l) < 0){
	printf("error making a link operation\n");
    }
}

int main( int argc, const char* argv[])
{
    struct link l;
    if(argc < 2){
      printf("usage: %s [o | c | [ [a|d]  wtap_id1  wtap_id2]]\n", argv[0]);
      return -1;
    }
    
    dev = open("/dev/visctl", O_RDONLY);
    if(dev < 0){
      printf("error opening visctl cdev\n");
      return -1;
    }
    
    char cmd = (char)*argv[1];
    
    switch(cmd){
      case 'o':
	toggle_medium(1);
	break;
      case 'c':
	toggle_medium(0);
	break;
      default:
	if(argc != 4){
	  printf("usage: %s [o | c | [ [a|d]  wtap_id1  wtap_id2]]\n", argv[0]);
	  return -1;
	}
	switch(cmd){
	  case 'a':
	    l.op = 1;
	    l.id1 = atoi(argv[2]);
	    l.id2 = atoi(argv[3]);
	    link_op(&l);
	    break;
	  case 'd':
	    l.op = 0;
	    l.id1 = atoi(argv[2]);
	    l.id2 = atoi(argv[3]);
	    link_op(&l);
	    break;
	  default:
	    printf("wtap ioctl: unkown command '%c'\n", *argv[1]);
	    return -1;
	}
    }
    return 0;
    
/*    
    int id = atoi(argv[2]);
    if(!(id >= 0 && id < 64)){
	printf("wtap_id must be between 0 and 7\n");
	return -1;
    }
    dev = open("/dev/visctl", O_RDONLY);
    if(dev < 0){
      printf("error opening wtapctl cdev\n");
      return -1;
    }
    switch((char)*argv[1]){
      case 'c':
	create(id);
	break;
      case 'd':
	delete(id);
	break;
      default:
	printf("wtap ioctl: unkown command '%c'\n", *argv[1]);
	return -1;
    }
    return 0;*/
}
