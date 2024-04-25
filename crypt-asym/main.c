#include "asym.h"


RSA *r = NULL;
BIGNUM *bne = NULL;
BIO *bp_public = NULL; 
BIO *bp_private = NULL;



int main(int argc, char** argv){

    if(argc < 2){
        fprintf(stderr, "too few arguments\n");
        return -1;
    }

    if(strcmp(argv[1], "keygen") == 0){


        char* priv_key_path = "./ca_priv.pem";
        char* pub_key_path = "./ca_pub.pem";
        int bits = 4096;
        
        int result = key_pair_generate(priv_key_path, pub_key_path, bits);   

        if(result < 0){
            fprintf(stderr,"%s failed: %d\n",argv[1], result);
            return result;
        } else {
            fprintf(stdout, "%s success\n", argv[1]);
        }

    } else if (strcmp(argv[1], "encrypt") == 0) {

        char* pub_key_path = "./ca_pub.pem";
        char* enc_path = "./enc_msg.bin";

        char* plain_msg = "cryptoinc";
        int plain_msg_len = strlen(plain_msg);

        int result = asym_encrypt(pub_key_path, enc_path, plain_msg_len, plain_msg);


    } else if (strcmp(argv[1], "decrypt") == 0) {

        char* priv_key_path = "./ca_priv.pem";
        char* pub_key_path = "./ca_pub.pem";
        char* enc_path = "./enc_msg.bin";

        char plain_msg[1024] = {0};
        

        int result = asym_decrypt(pub_key_path, priv_key_path, enc_path, plain_msg);

        printf("%s\n", plain_msg);

    } else if (strcmp(argv[1], "pipe") == 0){

        char* pub_key_path = "./ca_pub.pem";
        char* priv_key_path = "./ca_priv.pem";
        char* plain_msg = "cryptoinc";
        int plain_msg_len = strlen(plain_msg);

        int result = asym_pipe(pub_key_path, priv_key_path, plain_msg_len, plain_msg);

        printf("result: %d\n", result);

    } else if (strcmp(argv[1], "cert-verify") == 0){

        cert_verify();

    } else if (strcmp(argv[1], "cert-show") == 0){

        cert_show();   
    } else {

        fprintf(stderr, "invalid argument\n");
        return -10;
    }

    printf("done\n");

    return 0;
}