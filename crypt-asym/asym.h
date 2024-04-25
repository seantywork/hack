#ifndef _CRYPT_ASYM_H_
#define _CRYPT_ASYM_H_




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/bio.h>





extern RSA *r;
extern BIGNUM *bne;
extern BIO *bp_public;
extern BIO *bp_private;



int key_pair_generate(char* priv_key_path, char* pub_key_path, int bits);

int asym_encrypt(char* pub_key_path, char* enc_msg_path, int msg_len, char* msg);

int asym_decrypt(char* pub_key_path, char* priv_key_path, char* enc_msg_path, char* plain_msg);

int asym_pipe(char* pub_key_path, char* priv_key_path, int msg_len, char* msg);




void cert_verify();

void cert_show();


int sig_verify(BIO* cert_pem, BIO* intermediate_pem);

void cert_info(BIO* cert_pem);

unsigned char* char2hex(int arrlen, unsigned char* bytearray);

unsigned char* hex2char(unsigned char* hexarray);


void compare_two_arrays(int len, char* arr1, char* arr2);

void free_all();


#endif