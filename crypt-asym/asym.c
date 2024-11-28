#include "asym.h"



int key_pair_generate(char* priv_key_path, char* pub_key_path, int bits){


	int ret = 0;

	unsigned long e = RSA_F4;


	bne = BN_new();
	ret = BN_set_word(bne,e);
	if(ret != 1){
		free_all();
        return -1;
	}

	r = RSA_new();
	ret = RSA_generate_key_ex(r, bits, bne, NULL);
	if(ret != 1){
		free_all();
        return -2;
	}


	bp_private = BIO_new_file(priv_key_path, "w+");
	ret = PEM_write_bio_RSAPrivateKey(bp_private, r, NULL, NULL, 0, NULL, NULL);
	if(ret != 1){
		free_all();
        return -3;
	}


	bp_public = BIO_new_file(pub_key_path, "w+");
	ret = PEM_write_bio_RSAPublicKey(bp_public, r);
	if(ret != 1){
		free_all();
        return -4;
	}

    free_all();


    return 0;
}


int asym_encrypt(char* pub_key_path, char* enc_msg_path, int msg_len, char* msg){

    int result;

    FILE* fp;
    EVP_PKEY* pub_key = NULL;

    char* enc_msg = NULL;

    int enc_len = 0;

    char* err;

    fp = fopen(pub_key_path, "r");

    pub_key = PEM_read_PUBKEY(fp, NULL, NULL, NULL);

    fclose(fp);


    RSA* rsa_pub_key = EVP_PKEY_get1_RSA(pub_key);

    enc_msg = (char*)malloc(RSA_size(rsa_pub_key));

    err = (char*)malloc(130 * sizeof(char));

    enc_len = RSA_public_encrypt(
                    msg_len + 1, 
                    (unsigned char*)msg,
                    (unsigned char*)enc_msg,
                    rsa_pub_key,
                    RSA_PKCS1_OAEP_PADDING);

    printf("%d\n", enc_len);

    unsigned char* enc_hex = char2hex(enc_len, (unsigned char*)enc_msg);

    fp = fopen(enc_msg_path, "w");

    fputs((char*)enc_hex, fp);

    fclose(fp);

    RSA_free(rsa_pub_key);
    EVP_PKEY_free(pub_key);
    free(enc_msg);
    free(err);

    free(enc_hex);

    return 0;
}

int asym_decrypt(char* pub_key_path, char* priv_key_path, char* enc_msg_path, char* plain_msg){

    FILE* fp;
    EVP_PKEY* priv_key = NULL;


    char* enc_msg = NULL;

    int enc_len = 0;

    char* dec_msg = NULL;

    int dec_len = 0;

    char* err;

    fp = fopen(priv_key_path, "r");

    priv_key = PEM_read_PrivateKey(fp, NULL, NULL, NULL);

    fclose(fp);


    RSA* rsa_priv_key = EVP_PKEY_get1_RSA(priv_key);


    int _max_key_len = RSA_size(rsa_priv_key);

    enc_len = _max_key_len * 2 + 1;

    enc_msg = (char*)malloc(enc_len * sizeof(char));

    fp = fopen(enc_msg_path, "r");

    fgets(enc_msg, enc_len, fp);

    fclose(fp);

    unsigned char* dec_bin = hex2char((unsigned char*)enc_msg);

    dec_msg = (char*)malloc(RSA_size(rsa_priv_key));

    err = (char*)malloc(130 * sizeof(char));

    dec_len = RSA_private_decrypt(
                _max_key_len,
                dec_bin,
                (unsigned char*)dec_msg,
                rsa_priv_key,
                RSA_PKCS1_OAEP_PADDING
                );



    strcpy(plain_msg, dec_msg);

    RSA_free(rsa_priv_key);
    EVP_PKEY_free(priv_key);
    free(enc_msg);
    free(dec_msg);
    free(err);

    free(dec_bin);

    return 0;
}


int asym_pipe(char* pub_key_path, char* priv_key_path, int msg_len, char* msg){

    int result;

    FILE* fp;
    EVP_PKEY* pub_key = NULL;
    EVP_PKEY* priv_key = NULL;

    char* enc_msg = NULL;

    int enc_len = 0;

    char* dec_msg = NULL;

    int dec_len = 0;

    char* errmsg;

    fp = fopen(pub_key_path, "r");

    pub_key = PEM_read_PUBKEY(fp, NULL, NULL, NULL);

    fclose(fp);

    fp = fopen(priv_key_path, "r");

    priv_key = PEM_read_PrivateKey(fp, NULL, NULL, NULL);

    fclose(fp);

    RSA* rsa_pub_key = EVP_PKEY_get1_RSA(pub_key);

    RSA* rsa_priv_key = EVP_PKEY_get1_RSA(priv_key);

    int _enc_max = RSA_size(rsa_pub_key);

    int _dec_max = RSA_size(rsa_priv_key);

    printf("enc: %d, dec: %d\n", _enc_max, _dec_max);

    enc_msg = (char*)malloc(_enc_max);

    dec_msg = (char*)malloc(_dec_max);

    errmsg = (char*)malloc(130 * sizeof(char));

    enc_len = RSA_public_encrypt(
                    msg_len + 1, 
                    (unsigned char*)msg,
                    (unsigned char*)enc_msg,
                    rsa_pub_key,
                    RSA_PKCS1_OAEP_PADDING);

    printf("done\n");

    unsigned char* enc_hex = char2hex(enc_len, (unsigned char*)enc_msg);
   
    unsigned char* dec_bin = hex2char(enc_hex);

    compare_two_arrays(enc_len, enc_msg, dec_bin);

    dec_len = RSA_private_decrypt(
                enc_len,
                dec_bin,
                (unsigned char*)dec_msg,
                rsa_priv_key,
                RSA_PKCS1_OAEP_PADDING
                );

    printf("%d: %s\n", dec_len, dec_msg);


    RSA_free(rsa_pub_key);
    RSA_free(rsa_priv_key);  
    EVP_PKEY_free(pub_key);
    EVP_PKEY_free(priv_key);
    free(enc_msg);  
    free(dec_msg);
    free(errmsg); 
    free(enc_hex);
    free(dec_bin);
    return 0;
}


void cert_verify(){

    OpenSSL_add_all_algorithms();
    OpenSSL_add_all_ciphers();
    OpenSSL_add_all_digests(); 

    BIO* cert = NULL;
    BIO* intermediate = NULL;


    cert = BIO_new(BIO_s_file());

    intermediate = BIO_new(BIO_s_file());

    int ret = BIO_read_filename(cert, "./certs/server.pem");

    ret = BIO_read_filename(intermediate, "./certs/ca.pem");

    //cert_info(cert);
    //cert_info(intermediate);
    int res = sig_verify(cert,intermediate);
    printf("result: %d\n",res);


    BIO_free_all(cert);
    BIO_free_all(intermediate);


}

void cert_show(){


    OpenSSL_add_all_algorithms();
    OpenSSL_add_all_ciphers();
    OpenSSL_add_all_digests(); 

    BIO* cert = NULL;
    BIO* intermediate = NULL;


    cert = BIO_new(BIO_s_file());

    intermediate = BIO_new(BIO_s_file());

    int ret = BIO_read_filename(cert, "./certs/server.pem");

    ret = BIO_read_filename(intermediate, "./certs/ca.pem");

    cert_info(cert);
    cert_info(intermediate);



    BIO_free_all(cert);
    BIO_free_all(intermediate);


}

int sig_verify(BIO* cert_pem, BIO* intermediate_pem)
{
    //BIO *b = BIO_new(BIO_s_mem());
    //BIO_puts(b, intermediate_pem);

    BIO* b = intermediate_pem;
    X509 * issuer = PEM_read_bio_X509(b, NULL, NULL, NULL);
    EVP_PKEY *signing_key=X509_get_pubkey(issuer);

    //BIO *c = BIO_new(BIO_s_mem());
    //BIO_puts(c, cert_pem);
    BIO* c = cert_pem;
    X509 * x509 = PEM_read_bio_X509(c, NULL, NULL, NULL);
    
    int result = X509_verify(x509, signing_key);
    

    EVP_PKEY_free(signing_key);
    X509_free(x509);
    X509_free(issuer);
 
    return result;
}

void cert_info(BIO* cert_pem)
{
    //BIO *b = BIO_new(BIO_s_mem());
    //BIO_puts(b, cert_pem);
    BIO* b = cert_pem;
    X509 * x509 = PEM_read_bio_X509(b, NULL, NULL, NULL);

    BIO *bio_out = BIO_new_fp(stdout, BIO_NOCLOSE);
 
    BIO_printf(bio_out,"Subject: ");
    X509_NAME_print(bio_out,X509_get_subject_name(x509),0);
    BIO_printf(bio_out,"\n");

    BIO_printf(bio_out,"Issuer: ");
    X509_NAME_print(bio_out,X509_get_issuer_name(x509),0);
    BIO_printf(bio_out,"\n");
 

    //EVP_PKEY *pkey=X509_get_pubkey(x509);
    //EVP_PKEY_print_public(bio_out, pkey, 0, NULL);
    //EVP_PKEY_free(pkey);

    //X509_signature_print(bio_out, x509->sig_alg, x509->signature);
    //BIO_printf(bio_out,"\n");
 
    BIO_free(bio_out);
    X509_free(x509);
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


void compare_two_arrays(int len, char* arr1, char* arr2){


    for(int i = 0 ; i < len; i++){


        if(arr1[i] != arr2[i]){
            
            printf("not equal at: %d, %x %x\n", i, arr1[i], arr2[i]);

        }


    }



}



void free_all(){


	BIO_free_all(bp_public);
	BIO_free_all(bp_private);
	RSA_free(r);
	BN_free(bne);


}