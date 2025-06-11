#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <windows.h>
#include <shlobj.h>

#define MAX_TAREFAS 100
#define TAM_NOME 100
#define TAM_DESCRICAO 500

typedef struct {
    int dia;
    int mes;
    int ano;
} Data;

typedef struct {
    char nome[TAM_NOME];
    Data dataLimite;
    char descricao[TAM_DESCRICAO];
} Tarefa;

typedef struct {
    Tarefa tarefas[MAX_TAREFAS];
    int tamanho;
} ListaTarefas;

// Funções
void criarLista(const char *nomeArquivo);
void abrirLista(char *nomeArquivo, ListaTarefas *lista);
void salvarLista(const char *nomeArquivo, ListaTarefas *lista);
void listarTarefas(ListaTarefas *lista);
void adicionarTarefa(ListaTarefas *lista);
void editarTarefa(ListaTarefas *lista);
void removerTarefa(ListaTarefas *lista);
int compararDatas(Data a, Data b);
void inserirOrdenado(ListaTarefas *lista, Tarefa novaTarefa);

int main() {
    setlocale(LC_ALL, "Portuguese_Brasil.1252");
    ListaTarefas lista;
    lista.tamanho = 0;
    char nomeArquivo[100];
    int opcao;
    char desktopPath[MAX_PATH];
    char listasPath[MAX_PATH];
    HRESULT result = SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, desktopPath);
    if (result == S_OK) {
        snprintf(listasPath, sizeof(listasPath), "%s\\Listas", desktopPath);
        CreateDirectoryA(listasPath, NULL);
    } else {
        printf("Nao foi possivel acessar a Area de Trabalho.\n");
        return 1;
    }

    char buffer[100];

    while (1) {
        printf("1. Criar nova lista\n");
        printf("2. Abrir lista existente\n");
        printf("3. Apagar lista existente\n");
        printf("4. Sair do programa\n");
        printf("Escolha uma opcao (digite \"can\" durante a execucao de uma opcao para cancelar o procedimento): ");

        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strcmp(buffer, "can") == 0) {
            printf("Operacao cancelada.\n");
            continue; 
        }

        if (strlen(buffer) == 0) {
            printf("Erro: Nenhuma opcao digitada. Tente novamente.\n\n");
            continue;
        }

        opcao = atoi(buffer);

        if (opcao < 1 || opcao > 4) {
            printf("Erro: Opcao invalida. Tente novamente.\n\n");
            continue;
        }

        break;
    }

opcao = atoi(buffer);

if (opcao == 2 || opcao == 3) {
    printf("Listas disponíveis (.txt):\n");
    WIN32_FIND_DATAA findFileData;
    char searchPath[MAX_PATH];
    snprintf(searchPath, sizeof(searchPath), "%s\\*.txt", listasPath);
    HANDLE hFind = FindFirstFileA(searchPath, &findFileData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            printf("- %s\n", findFileData.cFileName);
        } while (FindNextFileA(hFind, &findFileData));
        FindClose(hFind);
    } else {
        printf("Nenhum arquivo .txt encontrado na pasta Listas.\n");
    }
}

printf("Digite o nome do arquivo (.txt) (digite \"can\" durante a execucao de uma opcao para cancelar o procedimento): ");
fgets(nomeArquivo, sizeof(nomeArquivo), stdin);
nomeArquivo[strcspn(nomeArquivo, "\n")] = '\0';

if (strcmp(nomeArquivo, "can") == 0) {
    printf("Operacao cancelada.\n");
    main();
    return 0;
}

if (strstr(nomeArquivo, ".txt") == NULL) {
    strcat(nomeArquivo, ".txt");
}
char caminhoCompleto[MAX_PATH];
snprintf(caminhoCompleto, sizeof(caminhoCompleto), "%s\\%s", listasPath, nomeArquivo);

switch (opcao) {
    case 1:
        while (1) {
            FILE *arquivoTeste = fopen(caminhoCompleto, "r");
            if (arquivoTeste != NULL) {
                fclose(arquivoTeste);
                printf("Ja existe um arquivo com esse nome na pasta Listas.\n");
                printf("Digite outro nome para o arquivo (.txt) (digite \"can\" durante a execucao de uma opcao para cancelar o procedimento): ");
                fgets(nomeArquivo, sizeof(nomeArquivo), stdin);
                nomeArquivo[strcspn(nomeArquivo, "\n")] = '\0';
                if (strcmp(nomeArquivo, "can") == 0) {
                    printf("Operacao cancelada.\n");
                    main();
                    return 0;
                }
                if (strstr(nomeArquivo, ".txt") == NULL) {
                    strcat(nomeArquivo, ".txt");
                }
                snprintf(caminhoCompleto, sizeof(caminhoCompleto), "%s\\%s", listasPath, nomeArquivo);
            } else {
                criarLista(caminhoCompleto);
                break;
            }
        }
        break;
    case 2:
        abrirLista(caminhoCompleto, &lista);
        break;
    case 3:
        if (remove(caminhoCompleto) == 0) {
            printf("Arquivo '%s' apagado com sucesso!\n", nomeArquivo);
        } else {
            printf("Erro ao apagar o arquivo '%s'.\n", nomeArquivo);
        }
        main();
        return 0;
    case 4: 
        printf("Programa encerrado.\n");
        return 0;
    default:
        printf("Opcao invalida!\n");
        return 1;
}


    while (1) {
        printf("\nMenu de Operacoes:\n");
        printf("1. Adicionar tarefa\n");
        printf("2. Editar tarefa\n");
        printf("3. Remover tarefa\n");
        printf("4. Listar tarefas\n");
        printf("5. Salvar e sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        getchar(); 

        switch (opcao) {
            case 1:
                adicionarTarefa(&lista);
                break;
            case 2:
                editarTarefa(&lista);
                break;
            case 3:
                removerTarefa(&lista);
                break;
            case 4:
                listarTarefas(&lista);
                break;
            case 5:
                salvarLista(caminhoCompleto, &lista);
                printf("Lista salva. Saindo...\n");
                return 0;
            default:
                printf("Opcao invalida!\n");
        }
    }

    return 0;
}

// Procedimentos
void criarLista(const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao criar o arquivo!\n");
        return;
    }
    fclose(arquivo);
    printf("Lista criada com sucesso!\n");
}

void abrirLista(char *nomeArquivo, ListaTarefas *lista) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    while (arquivo == NULL) {
        printf("Erro: O arquivo '%s' nao existe!\n", nomeArquivo);
        printf("Digite outro nome de arquivo para tentar abrir (.txt): ");
        fgets(nomeArquivo, 100, stdin);
        nomeArquivo[strcspn(nomeArquivo, "\n")] = '\0';
        if (strstr(nomeArquivo, ".txt") == NULL) {
            strcat(nomeArquivo, ".txt");
        }
        arquivo = fopen(nomeArquivo, "r");
    }

    lista->tamanho = 0;
    char linha[700];
    while (fgets(linha, sizeof(linha), arquivo)) {
        char nome[TAM_NOME];
        char data[15];
        char descricao[TAM_DESCRICAO];

        if (sscanf(linha, "%99[^\t]\t%14[^\t]\t%499[^\n]", nome, data, descricao) == 3) {
            strncpy(lista->tarefas[lista->tamanho].nome, nome, TAM_NOME);
            lista->tarefas[lista->tamanho].nome[TAM_NOME-1] = '\0';
            strncpy(lista->tarefas[lista->tamanho].descricao, descricao, TAM_DESCRICAO);
            lista->tarefas[lista->tamanho].descricao[TAM_DESCRICAO-1] = '\0';
            sscanf(data, "%d/%d/%d",
                   &lista->tarefas[lista->tamanho].dataLimite.dia,
                   &lista->tarefas[lista->tamanho].dataLimite.mes,
                   &lista->tarefas[lista->tamanho].dataLimite.ano);
            lista->tamanho++;
        }
    }
    fclose(arquivo);
    printf("Lista carregada com sucesso!\n");
}

void salvarLista(const char *nomeArquivo, ListaTarefas *lista) {
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao salvar o arquivo!\n");
        return;
    }
    int i;
    for (i = 0; i < lista->tamanho; i++) {
        fprintf(arquivo, "%s\t%02d/%02d/%04d\t%s\n",
                lista->tarefas[i].nome,
                lista->tarefas[i].dataLimite.dia,
                lista->tarefas[i].dataLimite.mes,
                lista->tarefas[i].dataLimite.ano,
                lista->tarefas[i].descricao);
    }
    fclose(arquivo);
}

void listarTarefas(ListaTarefas *lista) {
    printf("\n--------------------------------------------------\n");
    printf("Tarefas:\n");
    int i;
	for (i = 0; i < lista->tamanho; i++) {
        printf("%d. Nome: %s | Data Limite: %02d/%02d/%04d | Descricao: %s\n",
               i + 1,
               lista->tarefas[i].nome,
               lista->tarefas[i].dataLimite.dia,
               lista->tarefas[i].dataLimite.mes,
               lista->tarefas[i].dataLimite.ano,
               lista->tarefas[i].descricao);
    }
    printf("--------------------------------------------------\n");
}

void adicionarTarefa(ListaTarefas *lista) {
    if (lista->tamanho >= MAX_TAREFAS) {
        printf("A lista esta cheia!\n");
        return;
    }
    Tarefa novaTarefa;
    memset(&novaTarefa, 0, sizeof(Tarefa));
    int c;

    // Nome da tarefa
while (1) {
    printf("Digite o nome da tarefa (ou 'can' para cancelar): ");
    fgets(novaTarefa.nome, TAM_NOME, stdin);
    novaTarefa.nome[strcspn(novaTarefa.nome, "\n")] = '\0';
    if (strcmp(novaTarefa.nome, "can") == 0) {
        printf("Operacao cancelada.\n");
        return;
    }
    if (strlen(novaTarefa.nome) == 0) {
        printf("O nome da tarefa nao pode ficar em branco!\n");
        continue;
    }
    break;
}
    // Data limite
    while (1) {
    char dataBuffer[100];
    printf("Digite a data limite da tarefa (dd mm aaaa) (ou 'can' para cancelar): ");
    fgets(dataBuffer, sizeof(dataBuffer), stdin);
    dataBuffer[strcspn(dataBuffer, "\n")] = '\0';
    if (strcmp(dataBuffer, "can") == 0) {
        printf("Operacao cancelada.\n");
        return;
    }
    int campos = sscanf(dataBuffer, "%d %d %d", &novaTarefa.dataLimite.dia, &novaTarefa.dataLimite.mes, &novaTarefa.dataLimite.ano);
    if (campos != 3) {
        printf("Data invalida! Preencha todos os campos corretamente.\n");
        continue;
    }
    if (novaTarefa.dataLimite.dia <= 0 || novaTarefa.dataLimite.mes <= 0 || novaTarefa.dataLimite.ano <= 0) {
        printf("A data nao pode ficar em branco ou ser invalida!\n");
        continue;
    }
    break;
}

    // Descrição da tarefa
    while (1) {
        printf("Digite a descricao da tarefa (ate 500 caracteres): ");
        fgets(novaTarefa.descricao, TAM_DESCRICAO, stdin);
        novaTarefa.descricao[strcspn(novaTarefa.descricao, "\n")] = '\0';
        if (strlen(novaTarefa.descricao) == 0) {
            printf("A descricao nao pode ficar em branco!\n");
            continue;
        }
        break;
    }

    inserirOrdenado(lista, novaTarefa);
    printf("Tarefa adicionada com sucesso!\n");
}

void editarTarefa(ListaTarefas *lista) {
    if (lista->tamanho == 0) {
        printf("Nenhuma tarefa para editar.\n");
        return;
    }

    listarTarefas(lista);

    char buffer[100];
    printf("Digite o número da tarefa que deseja editar (ou 'can' para cancelar): ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    if (strcmp(buffer, "can") == 0) {
        printf("Operacao cancelada.\n");
        return;
    }
    int indice = atoi(buffer);

    if (indice < 1 || indice > lista->tamanho) {
        printf("Tarefa inválida!\n");
        return;
    }
    indice--;

    printf("\nO que deseja alterar?\n");
    printf("1. Nome\n");
    printf("2. Data Limite\n");
    printf("3. Descricao\n");
    printf("4. Alterar toda a tarefa\n");
    printf("Escolha uma opcao (ou 'can' para cancelar): ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    if (strcmp(buffer, "can") == 0) {
        printf("Operacao cancelada.\n");
        return;
    }
    int opcao = atoi(buffer);

    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    Tarefa temp = lista->tarefas[indice];

    switch (opcao) {
        case 1:
            printf("Digite o novo nome da tarefa: ");
            fgets(temp.nome, TAM_NOME, stdin);
            temp.nome[strcspn(temp.nome, "\n")] = '\0';
            break;
        case 2:
            printf("Digite a nova data limite (dd mm aaaa): ");
            scanf("%d %d %d", &temp.dataLimite.dia,
                               &temp.dataLimite.mes,
                               &temp.dataLimite.ano);
            while ((c = getchar()) != '\n' && c != EOF);
            break;
        case 3:
            printf("Digite a nova descricao: ");
            fgets(temp.descricao, TAM_DESCRICAO, stdin);
            temp.descricao[strcspn(temp.descricao, "\n")] = '\0';
            break;
        case 4:
            printf("Digite o novo nome da tarefa: ");
            fgets(temp.nome, TAM_NOME, stdin);
            temp.nome[strcspn(temp.nome, "\n")] = '\0';

            printf("Digite a nova data limite (dd mm aaaa): ");
            scanf("%d %d %d", &temp.dataLimite.dia,
                               &temp.dataLimite.mes,
                               &temp.dataLimite.ano);
            while ((c = getchar()) != '\n' && c != EOF);

            printf("Digite a nova descricao: ");
            fgets(temp.descricao, TAM_DESCRICAO, stdin);
            temp.descricao[strcspn(temp.descricao, "\n")] = '\0';
            break;
        default:
            printf("Opcao invalida!\n");
            return;
    }

    int i;
	for (i = indice; i < lista->tamanho - 1; i++) {
        lista->tarefas[i] = lista->tarefas[i + 1];
    }
    lista->tamanho--;

    inserirOrdenado(lista, temp);

    printf("Tarefa alterada com sucesso!\n");
}

void removerTarefa(ListaTarefas *lista) {
    if (lista->tamanho == 0) {
        printf("Nenhuma tarefa para remover.\n");
        return;
    }
    listarTarefas(lista);

    char buffer[100];
    printf("Digite o numero da tarefa que deseja remover (ou 'can' para cancelar): ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    if (strcmp(buffer, "can") == 0) {
        printf("Operacao cancelada.\n");
        return;
    }
    int indice = atoi(buffer);

    if (indice < 1 || indice > lista->tamanho) {
        printf("Tarefa inválida!\n");
        return;
    }
    int i;
    for (i = indice - 1; i < lista->tamanho - 1; i++) {
        lista->tarefas[i] = lista->tarefas[i + 1];
    }
    lista->tamanho--;

    printf("Tarefa removida com sucesso!\n");
}

int compararDatas(Data a, Data b) {
    if (a.ano != b.ano) return a.ano - b.ano;
    if (a.mes != b.mes) return a.mes - b.mes;
    return a.dia - b.dia;
}

void inserirOrdenado(ListaTarefas *lista, Tarefa novaTarefa) {
    int i = lista->tamanho - 1;
while (i >= 0 && compararDatas(novaTarefa.dataLimite, lista->tarefas[i].dataLimite) < 0) {
    lista->tarefas[i + 1] = lista->tarefas[i];
    i--;
	}
    lista->tarefas[i + 1] = novaTarefa;
    lista->tamanho++;
}
