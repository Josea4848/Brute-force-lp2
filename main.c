#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>

//Definições para encriptador
#define NUM_LETRAS 26
#define ASCII_A 65
#define QTD_PROCE 10 //<- QUANTIDADE DE SENHAS DO PROBLEMA
#define QTD_CHAR_FILE 12 //<- QUANTIDADE DE CARACTERES DOS ARQUIVOS DAS SENHAS (SENHAS/N.TXT)
#define SIZE_PSW 4 //Tamanho da senha

//Encriptador
char* encrypt(const char* str, int tamanho) {
    char* str_result = (char*) malloc(sizeof(char)*tamanho);
    for (int i = 0; i < tamanho; i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            int chave = str[i] - ASCII_A;
            str_result[i] = (str[i] - ASCII_A + chave) % NUM_LETRAS + ASCII_A;
        }
    }
    return str_result;
}

//Descriptografa senha
char* decrypt(const char* str, int tamanho) {
  //Aloca ponteiro de char que será a palavra
  char* word = (char *) malloc(sizeof(char)*tamanho);
  //Definindo seu último caracter
  word[tamanho] = '\0'; 

  //4 for para verificar
  for(word[0] = 'A'; word[0] <= 'Z'; word[0]++) 
    for(word[1] = 'A'; word[1] <= 'Z'; word[1]++) 
      for(word[2] = 'A'; word[2] <= 'Z'; word[2]++)
        for(word[3] = 'A'; word[3] <= 'Z'; word[3]++)
          if(!strcmp(encrypt(word, SIZE_PSW), str)) //Se a palavra encriptada foi igual a str, ela é a senha
            return word; //retorna endereço da palavra
}

int main() {
  int index, pids[QTD_PROCE], pid_0;
  char * senha1;

  //Armazenando pid do processo original
  pid_0 = getpid();

  //Crindo processos filhos
  for(index = 0; index < QTD_PROCE; index++) {
    if(getpid() == pid_0) {
      pids[index] = fork();

      if(!pids[index]) //Se for o filho
        break;
    }
  }

  //Se pai -> espera filhos descriptografarem
  if(getpid() == pid_0) {
    for(int i = 0; i < QTD_PROCE; i++) {
      int status;
      waitpid(pids[i], &status, 0);
    }
  } 
  
  else { //Se for filho, descriptografia
    FILE *senhaFile;
    FILE *saida;
    char dirFile[QTD_CHAR_FILE + 1] = "senhas/i.txt"; //Onde i é o index armazenado por cada Child 
    char dirOut[20] = "saidaN.txt"; //N representa o index do processo

    //Passando o index para o i
    dirFile[7] = '0' + index; 

    //Passando o index para o N
    dirOut[5] = '0' + index;

    //Abrindo arquivo com senhas criptografadas
    senhaFile = fopen(dirFile, "r");

    //Se o arquivo obteve erro ao abir, retorna erro
    if(senhaFile == NULL)
      exit(EXIT_FAILURE);

    //Abrindo arquivo de saída
    saida = fopen(dirOut, "w");

    if(saida == NULL)
      exit(EXIT_FAILURE);

    //Enquanto ler arquivo com senhas
    while(!feof(senhaFile)) {
      char* senha;
      char crpt_senha[SIZE_PSW + 1];

      //Lê linha
      fscanf(senhaFile, "%s\n", crpt_senha);

      //TRIM -> remove \n
      crpt_senha[SIZE_PSW] = '\0';

      //Passa senha descriptografada para senha
      senha = decrypt(crpt_senha, SIZE_PSW + 1);

      //Escreve a senha na saída
      fprintf(saida, "%s\n", senha);
    } 

    exit(EXIT_SUCCESS);
  }

  return 0;
}