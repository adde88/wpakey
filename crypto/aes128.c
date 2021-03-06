/* Ref. FIPS publication 197 Annoucing the Advanced Encryption Standard (AES)

   This implementation is based on the sample code from the publication
   and on tiny-AES128-C project https://github.com/kokke/tiny-AES128-C/

   Decryption and encryption in a single file for now.
   Would be better to untangle them at some point. */

#include <string.h>
#include "aes128.h"

static const unsigned Nk = 4;
static const unsigned Nb = 4;
static const unsigned Nr = 10;

static const uint8_t sbox[256] = {
0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
};

static const uint8_t rbox[256] = {
0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,
0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,
0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,
0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,
0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,
0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,
0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,
0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,
0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,
0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,
0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,
0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,
0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,
0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,
0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,
0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d
};

#undef BIGENDIAN

static const uint8_t rcon[11] = {
	0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

/* Bytes and words are *BIG*endian in AES */

#define B0(w) (((w) >> 24)       )
#define B1(w) (((w) >> 16) & 0xFF)
#define B2(w) (((w) >>  8) & 0xFF)
#define B3(w) (((w)      ) & 0xFF)

#define W(a,b,c,d) (((a) << 24) | ((b) << 16) | ((c) << 8) | (d))

static uint32_t rotword(uint32_t x)
{
	return ((x<<8) | ((x>>24) & 0xFF));
}

static uint32_t subword(uint32_t x, const uint8_t box[256])
{
	return W(box[B0(x)], box[B1(x)], box[B2(x)], box[B3(x)]);
}

static uint8_t xtime(uint8_t x)
{
	return ((x<<1) ^ (((x>>7) & 1) * 0x1b)) & 0xFF;
}

static uint8_t xmul(uint8_t x, uint8_t y)
{
	int m = 0;

	for(int i = 0; i <= 4; i++) {
		if(y & 1) m ^= x;
		y >>= 1;
		x = xtime(x);
	}

	return m & 0xFF;
}

static void add_round_key(uint32_t S[4], uint32_t W[44], int r)
{
	for(unsigned c = 0; c < Nb; c++)
		S[c] ^= W[r*Nb+c];
}

/* The original description uses column-first order, so their S(i, j)
   is byte(S[j], i) here. Otherwise this code follows the right side
   of Figure 13, P0-P3 being columns of their S'. */

static uint32_t rowshift(uint32_t S[4], int a, int b, int c, int d)
{
	return W(B0(S[a]), B1(S[b]), B2(S[c]), B3(S[d]));
}

static void inv_shift_rows(uint32_t S[4])
{
	uint32_t P0, P1, P2, P3;

	P0 = rowshift(S, 0, 3, 2, 1);
	P1 = rowshift(S, 1, 0, 3, 2);
	P2 = rowshift(S, 2, 1, 0, 3);
	P3 = rowshift(S, 3, 2, 1, 0);

	S[0] = P0; S[1] = P1; S[2] = P2; S[3] = P3;
}

static void fwd_shift_rows(uint32_t S[4])
{
	uint32_t P0, P1, P2, P3;

	P0 = rowshift(S, 0, 1, 2, 3);
	P1 = rowshift(S, 1, 2, 3, 0);
	P2 = rowshift(S, 2, 3, 0, 1);
	P3 = rowshift(S, 3, 0, 1, 2);

	S[0] = P0; S[1] = P1; S[2] = P2; S[3] = P3;
}

static uint8_t colmul(uint32_t Si, int a, int b, int c, int d)
{
	return xmul(B0(Si), a)
	     ^ xmul(B1(Si), b)
	     ^ xmul(B2(Si), c)
	     ^ xmul(B3(Si), d);
}

static void inv_sub_bytes(uint32_t S[4])
{
	for(int i = 0; i < 4; i++)
		S[i] = subword(S[i], rbox);
}

static void fwd_sub_bytes(uint32_t S[4])
{
	for(int i = 0; i < 4; i++)
		S[i] = subword(S[i], sbox);
}

static void inv_mix_columns(uint32_t S[4])
{
	for(int i = 0; i < 4; i++) {
		uint32_t Si = S[i];
		S[i] = W(colmul(Si, 0x0E, 0x0B, 0x0D, 0x09),
		         colmul(Si, 0x09, 0x0E, 0x0B, 0x0D),
		         colmul(Si, 0x0D, 0x09, 0x0E, 0x0B),
		         colmul(Si, 0x0B, 0x0D, 0x09, 0x0E));
	}
}

static void fwd_mix_columns(uint32_t S[4])
{
	for(int i = 0; i < 4; i++) {
		uint32_t Si = S[i];
		S[i] = W(colmul(Si, 0x02, 0x03, 0x01, 0x01),
		         colmul(Si, 0x01, 0x02, 0x03, 0x01),
		         colmul(Si, 0x01, 0x01, 0x02, 0x03),
		         colmul(Si, 0x03, 0x01, 0x01, 0x02));
	}
}

static void load_word(uint32_t* B, int i, const uint8_t in[4])
{
	B[i] = W(in[0], in[1], in[2], in[3]);
}

static void load_block(uint32_t B[4], const uint8_t in[16])
{
	load_word(B, 0, in + 0);
	load_word(B, 1, in + 4);
	load_word(B, 2, in + 8);
	load_word(B, 3, in + 12);
}

static void save_word(uint32_t w, uint8_t out[4])
{
	out[0] = B0(w);
	out[1] = B1(w);
	out[2] = B2(w);
	out[3] = B3(w);
}

static void save_block(uint32_t B[4], uint8_t out[16])
{
	save_word(B[0], out + 0*4);
	save_word(B[1], out + 1*4);
	save_word(B[2], out + 2*4);
	save_word(B[3], out + 3*4);
}

void aes128_init(struct aes128* ctx, const uint8_t key[16])
{
	uint32_t* W = ctx->W;
	uint32_t temp;
	unsigned Nw = Nb * (Nr + 1); /* 44, elements in W */
	unsigned i;

	load_block(W, key);

	for(i = 4; i < Nw; i++) {
		temp = W[i-1];

		if(i % Nk == 0) {
			temp = subword(rotword(temp), sbox);
			temp ^= ((uint32_t)rcon[i/Nk] << 24);
		}

		W[i] = W[i-Nk] ^ temp;
	}
}

void aes128_decrypt(struct aes128* ctx, uint8_t blk[16])
{
	uint32_t* W = ctx->W;
	uint32_t* S = ctx->S;
	unsigned r;

	load_block(S, blk);
	add_round_key(S, W, Nr);

	for(r = Nr - 1; r >= 1; r--) {
		inv_shift_rows(S);
		inv_sub_bytes(S);
		add_round_key(S, W, r);
		inv_mix_columns(S);
	};

	inv_shift_rows(S);
	inv_sub_bytes(S);
	add_round_key(S, W, 0);
	save_block(S, blk);
}

void aes128_encrypt(struct aes128* ctx, uint8_t blk[16])
{
	uint32_t* W = ctx->W;
	uint32_t* S = ctx->S;
	unsigned r;

	load_block(S, blk);
	add_round_key(S, W, 0);

	for(r = 1; r <= Nr - 1; r++) {
		fwd_sub_bytes(S);
		fwd_shift_rows(S);
		fwd_mix_columns(S);
		add_round_key(S, W, r);
	};

	fwd_sub_bytes(S);
	fwd_shift_rows(S);
	add_round_key(S, W, Nr);
	save_block(S, blk);
}

void aes128_fini(struct aes128* ctx)
{
	memset(ctx, 0, sizeof(*ctx));
}
