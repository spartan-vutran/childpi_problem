CC = gcc # Flag for implicit rules
CFLAGS = -g -D_DEFAULT_SOURCE# Flag for implicit rules. Turn on debug info
LDLIBS = -lacl -pthread # Flag for implicit rules. Lik acl library


BUILD_DIR := ./build
SERVER_DIR := ./server
CLIENT_DIR := ./client
INCLUDE_PATH := include

# INCLUDE FILE
SERVER_INCL := $(shell find $(SERVER_DIR)/$(INCLUDE_PATH) -name '*.c')
CLIENT_INCL := $(shell find $(CLIENT_DIR)/$(INCLUDE_PATH) -name '*.c')

# OBJECT FILE
SERVER_INCL_OBJS := $(SERVER_INCL:%.c=%.o)
CLIENT_INCL_OBJS := $(CLIENT_INCL:%.c=%.o)

# BUILD
build: ${BUILD_DIR}/server ${BUILD_DIR}/client

srvbuild: ${BUILD_DIR}/server

cltbuild: ${BUILD_DIR}/client

${BUILD_DIR}/server: ${SERVER_DIR}/server.o ${SERVER_INCL_OBJS}
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

${BUILD_DIR}/client: ${CLIENT_DIR}/client.o ${CLIENT_INCL_OBJS}
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@



.PHONY: clean

clean: cltclear srvclear

cltclear:
	rm -f ${CLIENT_DIR}/*.o $(CLIENT_DIR)/$(INCLUDE_PATH)/*.o ${BUILD_DIR}/client

srvclear:
	rm -f ${SERVER_DIR}/*.o $(SERVER_DIR)/$(INCLUDE_PATH)/*.o ${BUILD_DIR}/server