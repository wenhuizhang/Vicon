# $Author: luis $
# $Name:  $
# $Id: Makefile,v 1.1.1.1 2008-05-05 07:07:57 luis Exp $
# $Source: /home/luis/cvsroot/net-api/Makefile,v $
# $Log: Makefile,v $
# Revision 1.1.1.1  2008-05-05 07:07:57  luis
# initial import
#
# Revision 1.1.1.1  2008-05-05 06:49:35  luis
# initial import
#
# Revision 1.2  2006/01/18 15:47:45  cvs
# *** empty log message ***
#
# Revision 1.1.1.1  2005/11/06 10:56:35  cvs
# initial creation
#
# Revision 1.1.1.1  2005/10/22 16:44:21  cvs
# initial creation
#
# Revision 1.2  2005/10/18 16:30:20  cvs
# *** empty log message ***
#
# Revision 1.1.1.1  2005/10/15 15:14:06  cvs
# initial project comments
#
# Revision 1.1.1.1  2004/03/03 11:03:06  srik
# Initial Release 0.1
#
#

CC = gcc 
LD = $(CC)

LIBDIR = ../udp/
IMUDIR = ../../imu

CFLAGS = -O2  
LDFLAGS = -L./ -L$(LIBDIR) -lstate -lm 
LIB = libstate.a

SRCS = src/imuServerTester.c
OBJS = obj/imuServerTester.o $(IMUDIR)/obj/imuserial.o
INC = -I $(HELICONNECT) -I include -I $(IMUDIR)/include

SERVER = server

obj/%.o: src/%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(SERVER): $(OBJS) $(LIB)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(LIB): 
	cd $(LIBDIR); make $(LIB)

.phony: all clean

all:  clean $(LIB) $(SERVER)

clean:
	rm -f obj/* $(LIBDIR)*.a $(SERVER)
