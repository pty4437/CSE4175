#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bitset>

FILE* open_file, *output_file;
int data_word = 0;
int generator[9] = {0,};
int gen_size = 0;

void modulo(int *arr){
	int i;

	for(i = data_word*2 - 1; i >= gen_size; i--){
		if(arr[i] == 1){
			for(int j = i; j > i - gen_size; j--){
				arr[j] -= generator[i - j + 1];
				if(arr[j] == -1) arr[j] = 1;
			}
		}
	}

}

void make_binary(int* res, char c){
	int ascii = (int)c + 128;

	if(data_word == 4){
		int tmp1[8] = {0, };
		int tmp2[8] = {0, };

		//tmp1 : 첫 four bit,, tmp2 : 두번째 four bit
		for(int i = data_word; i < data_word*2; i++){
			tmp1[i] = ascii % 2;
			res[i] = tmp1[i];
			ascii /= 2;
		}

		for(int i = data_word; i < data_word * 2; i++){
			tmp2[i] = ascii % 2;
			res[i+7] = tmp2[i];
			ascii /= 2;
		}


		modulo(tmp1);
		for(int i = 1; i <= 3; i++) res[4-i] = tmp1[4 - i];
		modulo(tmp2);
		for(int i = 1; i <= 3; i++) res[11-i] = tmp2[4 - i];


	}
	else if(data_word == 8){
		int tmp[16] = {0,};

		for(int i = data_word; i < data_word * 2; i++){
			tmp[i] = ascii % 2;
			res[i] = tmp[i];
			ascii /= 2;
		}


		modulo(tmp);

		for(int i = 1; i <= 7; i++) res[8-i] = tmp[8 - i];

	}

}

void encoding(){
	char line[500];
	char space = ' ';

	//한줄씩 읽기
	while(fgets(line, sizeof(line), open_file) != NULL){

		for(int i = 0;;i++){
			int data[17] = {0,};
			uint8_t res1 = 0;
			uint8_t res2 = 0;
		        uint8_t bin = 1;	
			int j = 0;

			if(line[i] == '\0'){
				break;
			}

			make_binary(data, line[i]);
			for(int i = 9; i <=16; i++){
				res1 += bin * data[i];
				bin *= 2;
			}

			bin = 1;

			for(int i = 1; i <= 8; i++){
				res2 += bin * data[i];
				bin *= 2;
			}


			fwrite(&res1, sizeof(uint8_t), 1, output_file);
			fwrite(&res2, sizeof(uint8_t), 1, output_file);

		}
	}

}

int main(int argc, char *argv[]){

	//인자의 수가 맞지 않음
	if(argc != 5){
		printf("usage: ./crc_encoder input_file output_file generator dataword_size\n");
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

	data_word = atoi(argv[4]);

	//data word사이즈 이상한 경우
	if(data_word != 4 && data_word != 8){
		printf("dataword size must be 4 or 8.\n");
		exit(0);
	}
	else{
		uint8_t pad1 = 2, pad2 = 1;

		if(data_word == 4)
			fwrite(&pad1, sizeof(uint8_t), 1, output_file);
			//fprintf(output_file, "00000010 ");
		if(data_word == 8)
			fwrite(&pad2, sizeof(uint8_t), 1, output_file);
                        //fprintf(output_file, "00000001 ");

	}

	for(int i = 0;; i++){
		if(argv[3][i] == '\0') break;
		else generator[i+1] = argv[3][i] - 48;
		gen_size++;
	}


	encoding();
	fclose(open_file); fclose(output_file);
}
