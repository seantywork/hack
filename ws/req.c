
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "toyws.h"

int main(void)
{
	struct tws_ctx ctx;
	char msg[] = "Hello";


	char *buff;
	int frm_type;
	size_t buff_size;

	buff      = NULL;
	buff_size = 0;
	frm_type  = 0;

	if (tws_connect(&ctx, "127.0.0.1", 8080) < 0)
		fprintf(stderr, "Unable to connect!\n");


	printf("Send: %s\n",
		(tws_sendframe(&ctx, (uint8_t*)msg, strlen(msg), FRM_TXT) >= 0 ?
			"Success" : "Failed"));


	if (tws_receiveframe(&ctx, &buff, &buff_size, &frm_type) < 0)
		fprintf(stderr, "Unable to receive message!\n");

	printf("I received: (%s) (type: %s)\n", buff,
		(frm_type == FRM_TXT ? "Text" : "Binary"));

	tws_close(&ctx);

	free(buff);
	return (0);
}
