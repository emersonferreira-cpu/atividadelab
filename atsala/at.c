#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    char nome[100];
    float salario;
} Funcionario;

int main() {
    FILE *arquivo;
    Funcionario f;

    arquivo = fopen("funcionarios.txt", "a");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    printf("ID: ");
    scanf("%d", &f.id);

    getchar();

    printf("Nome: ");
    fgets(f.nome, sizeof(f.nome), stdin);

    printf("Salario: ");
    scanf("%f", &f.salario);

    fprintf(arquivo, "ID: %d\n", f.id);
    fprintf(arquivo, "Nome: %s", f.nome);
    fprintf(arquivo, "Salario: %.2f\n", f.salario);
    fprintf(arquivo, "-------------------------\n");

    fclose(arquivo);

    printf("Funcionario salvo com sucesso!\n");

    return 0;
}
   