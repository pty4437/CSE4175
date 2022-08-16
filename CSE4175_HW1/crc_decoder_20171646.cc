#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bitset>

FILE* open_file, *output_file, *result_file;
int data_word = 0;
int padding = 0;
int generator[9] = {0,};
int gen_size = 0;
int total_codeword = 0, error_codeword = 0;

void modulo(int *arr){
        int i;
        total_codeword++;

        for(i = data_word*2 - 1; i >= gen_size; i--){
                if(arr[i] == 1){
                        for(int j = i; j > i - gen_size; j--){
                                arr[j] -= generator[i - j + 1];
                                if(arr[j] == -1) arr[j] = 1;
                        }
                }
        }

        for(i = 1; i <= data_word * 2 - 1; i++){
                if(arr[i] == 1) {
                        error_codeword++;
                        break;
                }
        }

}


void write_codeword(int *arr){
	int ascii = 0;
	int binary = 1;


	if(data_word == 4){
		int code1[8] = {0,};
		int code2[8] = {0,};

		for(int i = 13; i >= 10; i--){
			ascii += arr[i] * binary;
			binary *= 2;	
		}

		for(int i = 6; i >= 3; i--){
			ascii += arr[i] * binary;
			binary *= 2;
	
		}


		fprintf(output_file, "%c", ascii-128);

		for(int i = 3; i <= 9; i++) code1[10-i] = arr[i];
		for(int i = 10; i <= 16; i++) code2[17-i] = arr[i];

		modulo(code1);
		modulo(code2);
	}
	else if(data_word == 8){
		int code[17] = {0, };

		for(int i = 9; i >= 2; i--){
			ascii += arr[i] * binary;
			binary *= 2;
		}

		fprintf(output_file, "%c", ascii-128);

		for(int i = 2; i <= 16; i++) code[17-i] = arr[i];

		modulo(code);
	}

}


void decoding(){
	char c;
	char buffer;
	int lsize;
	size_t result;
	int start = 9;
	int ascii;

	result = fread(&buffer, sizeof(char), 1, open_file);
	ascii = (int)buffer;
	
	if(data_word == 4) {
                padding = 2;
        }
        else {
                padding = 1;
        }

	if(ascii != padding)
		error_codeword++;


	while(feof(open_file) == 0){
		int idx;
		int bit[17] = {0,};

		result = fread(&buffer, sizeof(char), 1, open_file);
		
		if(feof(open_file) != 0) break;


		ascii = (int)buffer;
		if(ascii < 0)
			ascii = 256 + ascii;

		for(idx = 8; idx >= 1; idx--){
			bit[idx] = ascii % 2;
			ascii /= 2;
		}

		result = fread(&buffer, sizeof(char), 1, open_file);
                ascii = (int)buffer;

		if(ascii < 0)
                        ascii = 256 + ascii;


		for(idx = 16; idx >= 9; idx--){
			bit[idx] = ascii % 2;
			ascii /= 2;
		}


		write_codeword(bit);
	}




}

int main(int argc, char *argv[]){

        //인자의 수가 맞지 않음
        if(argc != 6){
                printf("usage: ./crc_decoder input_file output_file result_file generator dataword_size\n");
                exit(0);
        }

        open_file = fopen(argv[1], "r+");

        //입력 파일 존재하지 않는 경우
        if(open_file == NULL){
                printf("input file open error.\n");
                exit(0);
        }

        output_file = fopen(argv[2], "r+");

        //출력 파일 존재하지 않는 경우...??(처리 필요)
        if(output_file == NULL){
                printf("output file open error.\n");
                exit(0);
        }

	result_file = fopen(argv[3], "r+");

	//결과 파일..??
	if(result_file == NULL){
		printf("result file open error.\n");
		exit(0);
	}

	for(int i = 0;; i++){
                if(argv[4][i] == '\0') break;
                else generator[i+1] = argv[4][i] - 48;
                gen_size++;
        }

        data_word = atoi(argv[5]);

        //data word사이즈 이상한 경우
        if(data_word != 4 && data_word != 8){
                printf("dataword size must be 4 or 8.\n");
                exit(0);
        }

	decoding();

	fprintf(result_file, "%d %d", total_codeword, error_codeword);
	fclose(open_file); fclose(output_file); fclose(result_file);
}

