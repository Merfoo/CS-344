CXX = gcc
CXX_FLAGS = -std=c99 -Wall

# keygen
KEYGEN_EXE = keygen
KEYGEN_HDRS = Keygen.h
KEYGEN_SRCS = $(KEYGEN_HDRS:.h=.c)
KEYGEN_OBJS = $(KEYGEN_SRCS:.c=.o)

# otp_enc_d
ENCD_EXE = otp_enc_d
ENCD_HDRS = EncryptServer.h SocketUtil.h
ENCD_SRCS = $(ENCD_HDRS:.h=.c)
ENCD_OBJS = $(ENCD_SRCS:.c=.o)

# otp_enc
ENC_EXE = otp_enc
ENC_HDRS = EncryptClient.h SocketUtil.h Client.h
ENC_SRCS = $(ENC_HDRS:.h=.c)
ENC_OBJS = $(ENC_SRCS:.c=.o)

# otp_dec_d
DECD_EXE = otp_dec_d
DECD_HDRS = DecryptServer.h SocketUtil.h
DECD_SRCS = $(DECD_HDRS:.h=.c)
DECD_OBJS = $(DECD_SRCS:.c=.o)

# otp_dec
DEC_EXE = otp_dec
DEC_HDRS = DecryptClient.h SocketUtil.h Client.h
DEC_SRCS = $(DEC_HDRS:.h=.c)
DEC_OBJS = $(DEC_SRCS:.c=.o)

.PHONY: all clean

all: $(KEYGEN_EXE) $(ENCD_EXE) $(ENC_EXE) $(DECD_EXE) $(DEC_EXE)

# keygen
$(KEYGEN_EXE): $(KEYGEN_OBJS)
	$(CXX) $(CXX_FLAGS) $(KEYGEN_OBJS) -o $@

$(KEYGEN_OBJS): $(KEYGEN_HDRS)

# otp_enc_d
$(ENCD_EXE): $(ENCD_OBJS)
	$(CXX) $(CXX_FLAGS) $(ENCD_OBJS) -o $@

$(ENCD_OBJS): $(ENCD_HDRS)

# otp_enc
$(ENC_EXE): $(ENC_OBJS)
	$(CXX) $(CXX_FLAGS) $(ENC_OBJS) -o $@

$(ENC_OBJS): $(ENC_HDRS)

# otp_dec_d
$(DECD_EXE): $(DECD_OBJS)
	$(CXX) $(CXX_FLAGS) $(DECD_OBJS) -o $@

$(DECD_OBJS): $(DECD_HDRS)

# otp_dec
$(DEC_EXE): $(DEC_OBJS)
	$(CXX) $(CXX_FLAGS) $(DEC_OBJS) -o $@

$(DEC_OBJS): $(DEC_HDRS)

%.o: %.c
	$(CXX) $(CXX_FLAGS) -c $< -o $@

clean:
	rm -f 	$(KEYGEN_OBJS) $(KEYGEN_EXE)\
			$(ENCD_OBJS) $(ENCD_EXE)\
			$(ENC_OBJS) $(ENC_EXE)\
			$(DECD_OBJS) $(DECD_EXE)\
			$(DEC_OBJS) $(DEC_EXE)
