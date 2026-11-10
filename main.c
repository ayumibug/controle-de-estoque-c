
#include <stdio.h>
#include <string.h>

void adicionarItem();
void removerItem();
void listarEstoque();

int lerInteiro(char mensagem[]);
int lerQuantidade();
void lerNome(char nome[]);
void removerQuebraLinha(char texto[]);
int substituirArquivo();

int main() {

    int opcao;
    int continuar = 1;
    char resposta[20];

    while (continuar == 1) {

        printf("================================\n");
        printf("      Controle de Estoque\n");
        printf("================================\n");
        printf("Selecione uma opcao:\n");
        printf("1. Adicionar Item\n");
        printf("2. Remover Item\n");
        printf("3. Listar Estoque\n");
        printf("4. Sair\n");

        opcao = lerInteiro("Opcao: ");

        if (opcao < 1 || opcao > 4) {
            printf("Opcao invalida! Tente novamente.\n\n");
            continue;
        }

        switch (opcao) {

            case 1:
                adicionarItem();
                break;

            case 2:
                removerItem();
                break;

            case 3:
                listarEstoque();
                break;

            case 4:
                continuar = 0;
                break;
        }

        if (opcao != 4) {

            printf("\nDeseja realizar outra operacao? (s/n): ");

            if (fgets(resposta, sizeof(resposta), stdin) == NULL) {
                continuar = 0;
            } else if (resposta[0] == 's' || resposta[0] == 'S') {
                continuar = 1;
            } else {
                continuar = 0;
            }
        }
    }

    printf("\nObrigado por usar o Controle de Estoques! Ate a proxima.\n");

    return 0;
}


int lerInteiro(char mensagem[]) {

    char entrada[100];
    int valor;
    char extra;

    while (1) {

        printf("%s", mensagem);

        if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
            return -1;
        }

        if (sscanf(entrada, " %d %c", &valor, &extra) == 1) {
            return valor;
        }

        printf("Entrada invalida! Digite um numero.\n");
    }
}


int lerQuantidade() {

    int quantidade;

    do {
        quantidade = lerInteiro("Digite a quantidade: ");

        if (quantidade <= 0) {
            printf("Digite uma quantidade maior que zero.\n");
        }

    } while (quantidade <= 0);

    return quantidade;
}


void lerNome(char nome[]) {

    do {
        printf("Digite o nome do item: ");

        if (fgets(nome, 100, stdin) == NULL) {
            nome[0] = '\0';
            return;
        }

        removerQuebraLinha(nome);

        if (strlen(nome) == 0) {
            printf("O nome nao pode ficar vazio.\n");
        }

    } while (strlen(nome) == 0);
}


void removerQuebraLinha(char texto[]) {

    texto[strcspn(texto, "\n")] = '\0';
}


int substituirArquivo() {

    if (remove("inventory.txt") != 0) {
        return 0;
    }

    if (rename("temp.txt", "inventory.txt") != 0) {
        return 0;
    }

    return 1;
}


void adicionarItem() {

    char nome[100];
    char item[100];

    int quantidade;
    int qtdEstoque;
    int encontrado = 0;

    FILE *arquivo;
    FILE *temporario;

    lerNome(nome);

    quantidade = lerQuantidade();

    arquivo = fopen("inventory.txt", "r");

    if (arquivo == NULL) {

        arquivo = fopen("inventory.txt", "w");

        if (arquivo == NULL) {
            printf("Nao foi possivel atualizar o estoque.\n");
            return;
        }

        fprintf(arquivo, "%s\n%d\n", nome, quantidade);

        fclose(arquivo);

        printf("Item adicionado com sucesso!\n");

        return;
    }

    temporario = fopen("temp.txt", "w");

    if (temporario == NULL) {

        fclose(arquivo);

        printf("Nao foi possivel atualizar o estoque.\n");

        return;
    }

    while (fgets(item, sizeof(item), arquivo) != NULL) {

        removerQuebraLinha(item);

        if (fscanf(arquivo, "%d", &qtdEstoque) != 1) {
            break;
        }

        fgetc(arquivo);

        if (strcmp(nome, item) == 0 && encontrado == 0) {

            qtdEstoque += quantidade;

            encontrado = 1;
        }

        fprintf(temporario, "%s\n%d\n", item, qtdEstoque);
    }

    if (encontrado == 0) {

        fprintf(temporario, "%s\n%d\n", nome, quantidade);
    }

    fclose(arquivo);
    fclose(temporario);

    if (substituirArquivo()) {
        printf("Item adicionado com sucesso!\n");
    } else {
        printf("Nao foi possivel atualizar o estoque.\n");
    }
}


void removerItem() {

    char nome[100];
    char item[100];

    int quantidade;
    int qtdEstoque;
    int encontrado = 0;
    int insuficiente = 0;
    int removido = 0;

    FILE *arquivo;
    FILE *temporario;

    lerNome(nome);

    quantidade = lerQuantidade();

    arquivo = fopen("inventory.txt", "r");

    if (arquivo == NULL) {

        printf("Item nao encontrado.\n");

        return;
    }

    temporario = fopen("temp.txt", "w");

    if (temporario == NULL) {

        fclose(arquivo);

        printf("Nao foi possivel atualizar o estoque.\n");

        return;
    }

    while (fgets(item, sizeof(item), arquivo) != NULL) {

        removerQuebraLinha(item);

        if (fscanf(arquivo, "%d", &qtdEstoque) != 1) {
            break;
        }

        fgetc(arquivo);

        if (strcmp(nome, item) == 0 && encontrado == 0) {

            encontrado = 1;

            if (quantidade > qtdEstoque) {

                insuficiente = 1;

                fprintf(temporario, "%s\n%d\n", item, qtdEstoque);

            } else {

                qtdEstoque -= quantidade;

                if (qtdEstoque == 0) {

                    removido = 1;

                } else {

                    fprintf(temporario, "%s\n%d\n", item, qtdEstoque);
                }
            }

        } else {

            fprintf(temporario, "%s\n%d\n", item, qtdEstoque);
        }
    }

    fclose(arquivo);
    fclose(temporario);

    if (encontrado == 0) {

        remove("temp.txt");

        printf("Item nao encontrado.\n");

    } else if (insuficiente == 1) {

        remove("temp.txt");

        arquivo = fopen("inventory.txt", "r");

        if (arquivo != NULL) {

            while (fgets(item, sizeof(item), arquivo) != NULL) {

                removerQuebraLinha(item);

                if (fscanf(arquivo, "%d", &qtdEstoque) != 1) {
                    break;
                }

                fgetc(arquivo);

                if (strcmp(nome, item) == 0) {
                    break;
                }
            }

            fclose(arquivo);
        }

        printf("Estoque insuficiente. Quantidade disponivel: %d\n",
               qtdEstoque);

    } else if (substituirArquivo()) {

        if (removido == 1) {

            printf("Item removido do estoque!\n");

        } else {

            printf("Quantidade atualizada com sucesso!\n");
        }

    } else {

        printf("Nao foi possivel atualizar o estoque.\n");
    }
}


void listarEstoque() {

    char item[100];
    int quantidade;
    int encontrou = 0;

    FILE *arquivo;

    arquivo = fopen("inventory.txt", "r");

    if (arquivo == NULL) {

        printf("O estoque esta vazio.\n");

        return;
    }

    printf("=============================\n");
    printf("        Estoque Atual\n");
    printf("=============================\n");

    while (fgets(item, sizeof(item), arquivo) != NULL) {

        removerQuebraLinha(item);

        if (fscanf(arquivo, "%d", &quantidade) != 1) {
            break;
        }

        fgetc(arquivo);

        printf("Nome: %s\n", item);
        printf("Quantidade: %d\n\n", quantidade);

        encontrou = 1;
    }

    if (encontrou == 0) {

        printf("O estoque esta vazio.\n");
    }

    fclose(arquivo);
}
