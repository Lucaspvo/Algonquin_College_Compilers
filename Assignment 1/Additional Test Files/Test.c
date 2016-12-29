int main() {

	FILE * fp;
	fp = fopen("TextFile1.txt", "r");

	b_create(10, 20, 'f');

	b_load(fp, pbuffer);

	fclose(fp);

//	b_addc(pbuffer, 'a');
//	b_addc(pbuffer, 'b');
//	b_addc(pbuffer, 'c');
//	//b_addc(pbuffer, 'd');
//	//b_addc(pbuffer, 'e');
//	/*b_addc(pbuffer, 'f');
//	b_addc(pbuffer, 'g');
//	b_addc(pbuffer, 'h');
//	b_addc(pbuffer, 'i');
//	b_addc(pbuffer, 'j');
//	b_addc(pbuffer, 'k');
//	b_addc(pbuffer, 'l');
//	b_addc(pbuffer, 'm');
//	b_addc(pbuffer, 'n');
//	b_addc(pbuffer, 'o');
//	b_addc(pbuffer, 'p');
//	b_addc(pbuffer, 'q');
//	b_addc(pbuffer, 'r');
//	b_addc(pbuffer, 's');
//	b_addc(pbuffer, 't');
//	b_addc(pbuffer, 'u');
//	b_addc(pbuffer, 'v');*/
//	b_print(pbuffer);
//	/*b_destroy(pbuffer);*/
//	/*printf("%c", b_getc(pbuffer));
//	printf("%c", b_getc(pbuffer));
//	printf("%c", b_getc(pbuffer));*/
//	//printf("%d\n", b_isempty(pbuffer));
//	
//	/*printf("Size: %d \n",b_size(pbuffer));
//	printf("test: %d \n",b_capacity(pbuffer));*/
//	/*printf("test: %d \n",pointerB->inc_factor);*/
	return 0;
}
