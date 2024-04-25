#include "sym.h"


int sym_keygen(char* key_path, char* iv_path){

    FILE* fp;


    unsigned char* key = gen_random_bytestream(KEYLEN);

    unsigned char* iv = gen_random_bytestream(IVLEN);

    unsigned char* hex_key = char2hex(KEYLEN, key);

    unsigned char* hex_iv = char2hex(IVLEN, iv);


    fp = fopen(key_path, "w");

    fputs(hex_key, fp);

    fclose(fp);

    fp = fopen(iv_path, "w");

    fputs(hex_iv, fp);

    fclose(fp);
    

    free(key);
    free(iv);
    
    free(hex_key);
    free(hex_iv);

    return 0;
}


int sym_encrypt(char* key_path, char* iv_path, int enc_len, char* enc_msg, char* enc_path){

    FILE* fp;

    EVP_CIPHER_CTX *ctx;

    int outlen, rv = 0;
    unsigned char outbuf[MAX_OUT] = {0};
    
    char gcm_key_hex[KEYLEN * 2 + 1];
    char gcm_iv_hex[IVLEN * 2 + 1];
    unsigned char gcm_tag[TAGLEN] = {0};

    unsigned char* gcm_key;

    unsigned char* gcm_iv;

    fp = fopen(key_path, "r");

    fgets(gcm_key_hex, KEYLEN * 2 + 1, fp);

    fclose(fp);


    fp = fopen(iv_path, "r");

    fgets(gcm_iv_hex, IVLEN * 2 + 1, fp);

    fclose(fp);

    gcm_key = hex2char((unsigned char*)gcm_key_hex);

    gcm_iv = hex2char((unsigned char*)gcm_iv_hex);


    ctx = EVP_CIPHER_CTX_new();

    EVP_EncryptInit(ctx, cipher, NULL, NULL);

    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IVLEN, NULL);

    EVP_EncryptInit(ctx, NULL, gcm_key, gcm_iv);

    // EVP_EncryptUpdate(ctx, NULL, &outlen, gcm_aad, sizeof(gcm_aad));

    EVP_EncryptUpdate(ctx, outbuf, &outlen, enc_msg, enc_len + 1);


    unsigned char* outbuf_hex = char2hex(outlen, outbuf);

    fp = fopen(enc_path, "w");

    fputs(outbuf_hex, fp);

    fclose(fp);

    rv = EVP_EncryptFinal(ctx, outbuf, &outlen);

    printf("encrypt rv: %d\n", rv);

    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TAGLEN, outbuf);

    memcpy(gcm_tag, outbuf, TAGLEN);

    
    unsigned char* tag_hex = char2hex(TAGLEN, gcm_tag);


    fp = fopen("./tag.data", "w");

    fputs(tag_hex, fp);

    fclose(fp);

    EVP_CIPHER_CTX_free(ctx);
    free(gcm_key);
    free(gcm_iv);

    free(outbuf_hex);
    free(tag_hex);

    return 0;

}



int sym_decrypt(char* key_path, char* iv_path, char* enc_path, char* dec_msg){

    FILE* fp;

    EVP_CIPHER_CTX *ctx;

	int outlen, tmplen, rv;

    char inbuf[MAX_OUT] = {0};


    char outbuf[MAX_OUT] = {0};

    
    char gcm_key_hex[KEYLEN * 2 + 1];
    char gcm_iv_hex[IVLEN * 2 + 1];

    char gcm_tag_hex[TAGLEN * 2 + 1];

    unsigned char* gcm_key;

    unsigned char* gcm_iv;

    unsigned char* gcm_tag;

    fp = fopen(key_path, "r");

    fgets(gcm_key_hex, KEYLEN * 2 + 1, fp);

    fclose(fp);


    fp = fopen(iv_path, "r");

    fgets(gcm_iv_hex, IVLEN * 2 + 1, fp);

    fclose(fp);

    fp = fopen("./tag.data", "r");

    fgets(gcm_tag_hex, TAGLEN * 2 + 1, fp);

    fclose(fp);

    gcm_key = hex2char((unsigned char*)gcm_key_hex);

    gcm_iv = hex2char((unsigned char*)gcm_iv_hex);

    gcm_tag = hex2char((unsigned char*)gcm_tag_hex);

    fp = fopen(enc_path, "r");

    fgets(inbuf, MAX_OUT, fp);

    fclose(fp);


    unsigned char* inbuf_bin = hex2char((unsigned char*)inbuf);


    ctx = EVP_CIPHER_CTX_new();


    EVP_DecryptInit(ctx, cipher, NULL, NULL);

    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IVLEN, NULL);

    EVP_DecryptInit(ctx, NULL, gcm_key, gcm_iv);

    //EVP_DecryptUpdate(ctx, NULL, &outlen, gcm_aad, sizeof(gcm_aad));

    EVP_DecryptUpdate(ctx, outbuf, &outlen, inbuf_bin, sizeof(inbuf_bin) + PADDING);

    printf("%d\n", outlen);

    strcpy(dec_msg, outbuf);

    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, TAGLEN, gcm_tag);

    rv = EVP_DecryptFinal_ex(ctx, outbuf, &outlen);

    printf("decrypt rv: %d\n", rv);


    EVP_CIPHER_CTX_free(ctx);
    free(gcm_key);
    free(gcm_iv);
    free(gcm_tag);

    free(inbuf_bin);
    return 0;
}


unsigned char* gen_random_bytestream (size_t num_bytes){
  
  unsigned char *stream = malloc (num_bytes);

  size_t i;

  for (i = 0; i < num_bytes; i++){

    stream[i] = rand();
  
  }

  return stream;
}





unsigned char* char2hex(int arrlen, unsigned char* bytearray){

    unsigned char* hexarray;

    int hexlen = 2;

    int outstrlen = hexlen * arrlen + 1;

    hexarray = (char*)malloc(outstrlen * sizeof(char));

    memset(hexarray, 0, outstrlen * sizeof(char));

    unsigned char* ptr = hexarray;

    for(int i = 0 ; i < arrlen; i++){

        sprintf(ptr + 2 * i, "%02X", bytearray[i]);

        printf("%d: %c%c ", i, ptr[2 * i], ptr[2 * i + 1]);
    }

    printf("\n");

    return hexarray;
}




unsigned char* hex2char(unsigned char* hexarray){

    unsigned char* chararray;

    int hexlen = strlen(hexarray);

    int outstrlen = hexlen  / 2;

    chararray = (char*)malloc(outstrlen * sizeof(char));

    memset(chararray, 0, outstrlen * sizeof(char));

    unsigned int n = 0;

    for(int i = 0 ; i < outstrlen; i++){

        sscanf(hexarray + 2 * i, "%2x", &n);

        chararray[i] = n;

        printf("%d: %c%c %d %x ", i, hexarray[2 * i], hexarray[2 * i + 1], n, chararray[i]);

    }

    printf("\n");

    return chararray;
}
