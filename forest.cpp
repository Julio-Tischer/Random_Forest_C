#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
//Codigo IA por: Julio Frederico Weingartner Tischer -- UTFPR s25 -- 2025
//tratamento de dados por: Lucas Henrique Hoffman -- UTFPR s25 -- 2025

//Para Zannetti: Eu peço desculpas se quiser mostrar o codigo para um gringo, eu programo metade em ingles metade em portugues ;P
//Tambem decidi que seria uma experiencia mais prazerosa de leitura se os comentarios originais fossem mantidos, se divirta!

//--------------------------------------------------------------------------------------------------------------------------//

//Coloque aqui as variaveis para as arvores
#define TREE_NUMBER 100
#define SAMPLE_SIZE 50
#define FEATURES_PER_TREE 50
#define MIN_SAMPLES_PER_NODE 15
#define SMALLEST_VALUE_OF_DATA -1000
#define NODE_LIMIT 48

//--------------------------------------------------------------------------------------------------------------------------//

//foreshadowing de funções (estilo AoT)
int checkRepeat (int* vector, int new_entry, int sizeOf_Vector);
int testStructure(struct Tree tree, double* testData, int featureNumber);
int* da_IntArr (int* variable, int size);
int* rndList(int sizeof_List, int maxNumber, int minNumber);

double* da_DBLArr (double* variable, int size);
double** sortSTG (double** matrizDados,int coluna, int numeroLinhas, int numeroFeatures);
double** sortSTG (double** matrizDados,int coluna, int numeroLinhas, int numeroFeatures);
double** da_DBLmtx (double** variable, int rows, int colunms);

void double_SuperFree(double** array, int columns);

struct Tree buildTree(double** dataMatrix, int* sample_List, int* feature_List, int feature_Number, int sample_Number);
struct giniOutput calcGini(int ID, double threshold, double best_Threshold, struct valueAnswer inputTable, int lenghtOf_Table);
struct ordemFeature sortByGini(struct ordemFeature input, int lineNumber, int featurenumber);
struct ordemFeature giniImpurity(int numeroLinhas, int numeroFeatures, double** matrizDados);
struct valueAnswer* sortFeature(double** dataMatrix, int feature_ID, int* sample_List, int sample_amount, int feature_Number);

//--------------------------------------------------------------------------------------------------------------------------//

//MEU DEUS STRUCTS SÃO MUITO TOPPPPPPPPPPPPPPP

//É o nó de uma arvore, contem todas informações de teste necessarias (substitui ordemFeature)
typedef struct node
{
    int test_ID;
    int isPositive;
    double threshold;
    double gini;
    int left_Node;
    int right_Node;
}TNode;

//Armazena uma coleção de 'galhos' (nós)(é uma arvore)
typedef struct Tree
{
    TNode* galhos;
}TTree;

//Esse struct armazena uma lista de ID com seus respectivos IDs e positividades (se abaixo de threshold é positivo ou falso)
struct ordemFeature 
{
    int* ID;
    double* threshold;
    int* isPositive;
    int* isLeaf;
    double* gini;
};

//Essa é estrutura (hehe) do output da função GINI
struct giniOutput
{
    int feature_ID;
    double impurity;
    double threshold;
    int isPositive;
};

typedef struct valueAnswer
{
    double value;
    int awnser;
}TValAws;


//--------------------------------------------------------------------------------------------------------------------------//

int main()
{
    srand(time(NULL));

    //Arquivos
    FILE* FILE_test = NULL;
    FILE* FILE_Train = NULL;

    int numeroFeatures=0;
    int numeroLinhas=0;
    int testeLinhas=0;

    //Matriz de double que armazena conteudo de uma dada linha e coluna
    double** dataTable = NULL;
    double** testTable = NULL;
    char charBuff;

    //Array de arvores (Vulgo uma floresta)
    TTree* forest;

    //Abrir FILEs
    FILE_test = fopen("test.csv","r");
    FILE_Train = fopen("train.csv","r");
    if (FILE_Train == NULL||FILE_test==NULL)
    {
        perror("\n--Erro abertura arquivos");
        getchar();
        exit (1);
    }

    //Contar quantidade de features 
    while (charBuff != '\n')
    {
        charBuff = fgetc(FILE_Train);
        if (charBuff == ','){numeroFeatures++;}
    }
    numeroFeatures++; //Como ultima feature não virgula, somamos 1
    printf("--%d Features detectadas\n",numeroFeatures);

    //Contar quantidade de linhas
    rewind(FILE_Train);
    charBuff='0';
    while (charBuff != EOF)
    {
        charBuff = fgetc(FILE_Train);
        if (charBuff == '\n'){numeroLinhas++;}
    }
    printf("--%d Linhas detectadas\n",numeroLinhas);
    rewind(FILE_Train);

    //Contar numero de linhas no arquivo de teste
    charBuff='0';
    while (charBuff != EOF)
    {
        charBuff = fgetc(FILE_test);
        if (charBuff == '\n'){testeLinhas++;}
    }
    printf("--%d Linhas detectadas no arquivo de teste\n",testeLinhas);
    rewind(FILE_test);

    //Alocar linhas e colunas
    dataTable = da_DBLmtx(dataTable, numeroLinhas, numeroFeatures);
    printf("-- Matriz de treino de tamanho[%d][%d] alocada\n",numeroLinhas,numeroFeatures);

    //Preencher a matriz com os valores -- Utilizei Copilot para me ajudar com o scanf
    for (int i=0; i<numeroLinhas;i++)
    {
        for(int j=0;j<numeroFeatures;j++)
        {
            //Se for a ultima feature, ler um double e consumir um \n
            if (j < numeroFeatures-1)
            {
                fscanf(FILE_Train,"%lf,",&dataTable[i][j]);
            } 
            else
            {
                fscanf(FILE_Train,"%lf\n",&dataTable[i][j]);
            }
            //printf("%d\t",dataTable[i][j]);
        }
    }

    //Criar um vetor para armazenar valores de teste
    testTable = da_DBLmtx(testTable, testeLinhas, numeroFeatures);
    printf("-- Matriz de teste de tamanho[%d][%d] alocada\n",testeLinhas,numeroFeatures);
    
    for (int i=0; i<testeLinhas;i++)
    {
        for(int j=0;j<numeroFeatures;j++)
        {
            //Se for a ultima feature, ler um double e consumir um \n
            if (j < numeroFeatures-1)
            {
                fscanf(FILE_test,"%lf,",&testTable[i][j]);
            } 
            else
            {
                fscanf(FILE_test,"%lf\n",&testTable[i][j]);
            }
            //printf("%d\t",testTable[i][j]);
        }
    }

    //----------------------------------------------------------------------------------------------//
    // Criar TREE_NUMBER arvores
    forest = (TTree*)malloc(TREE_NUMBER*sizeof(TTree));
    if (forest==NULL)
    {
        perror("\n--Erro alocação forest");
        getchar;
        exit(1);
    }

    //Para toda arvore, criar ela com sample e features aleatorias
    for (int i=0; i<TREE_NUMBER; i++)
    {
        int* local_Samples = rndList(SAMPLE_SIZE, numeroLinhas, 0);
        int* local_Features = rndList(FEATURES_PER_TREE, numeroFeatures-1, 0);

        forest[i] = buildTree(dataTable,local_Samples,local_Features,numeroFeatures,numeroLinhas);
        printf("\n\n//---------------------------------------------------//\n\t%d Arvore criada\n",i+1);

        free(local_Samples);
        free(local_Features);
    }

    //Para toda linha de teste
    int acertos=0;
    for (int i=0;i<testeLinhas;i++)
    {
        int positivos=0;
        int resposta=0;
        int invalidas=0;
        //Para todas as arvores
        for(int j=0;j<TREE_NUMBER;j++)
        {
            //Anotar quantas arvores foram positivas
            if(testStructure(forest[j],testTable[i],numeroFeatures)==1){positivos++;}

            //Anotar quantas arvores são invalidas
            else if(testStructure(forest[j],testTable[i],numeroFeatures)==-1){invalidas++;}
        }
        if (positivos>(TREE_NUMBER-invalidas)/2)
        {
            resposta=1;
        }
        else{resposta=0;}

        //Verificar se arvore acertou previsão
        if (resposta==testTable[i][numeroFeatures-1])
        {
            acertos++;
        }
        printf("\n--%d arvores de %d deram resposta positiva. %f%%\n. \n--Arvores invalidas:%d",positivos,TREE_NUMBER-invalidas,100.0*positivos/(TREE_NUMBER-invalidas),invalidas);
    }
    printf("\n\n--Foram feitos %d acertos de %d. Precisao final de %f%%",acertos,testeLinhas,100.0*acertos/testeLinhas);

    //---------------------------------------------------------------------------------------------//

    //Encerrção do codigo
    double_SuperFree(dataTable,numeroLinhas);
    //para terminal não fechar sozinho
    fclose(FILE_Train);
    fclose(FILE_test);
    getchar(); 
    return 0;
}

//Retorna 1 se new_entry estiver em vector
int checkRepeat (int* vector, int new_entry, int sizeOf_Vector)
{
    int flag=0;
    //Flag para anotar se numero repete

    for (int i=0; i<sizeOf_Vector; i++)
    {
        if (new_entry == vector[i])
        {
            flag=1;
            //printf("\n---NUMERO %d E REPETIDO---\n",new_entry);
        }
        else{}
    }

    //Se houver repetido, retorna 1;
    return flag;
}

//Função que calcula melhor gini dada uma lista de feature, use best_Threshold = -1 para condição inicial
struct giniOutput calcGini(int ID, double threshold, double best_Threshold, TValAws* inputTable, int lenghtOf_Table)
{
    struct giniOutput outputStruct;

    //Os contadores
    double counter_PrePos=0;
    double counter_PreFal=0;
    double counter_PosPos=0;
    double counter_PosFal=0;

    double gini_Pre =0.0;
    double gini_Pos =0.0;
    double gini_Final =0.0;
    double gini_Best =0.0;
    int is_Best =0;

    //Condição de inicio
    if (best_Threshold==-1)
    {
        //Percorra a mesa e incremente os contadores conforema necessario
        for (int i=0;i<lenghtOf_Table;i++)
        {
            if (inputTable[i].value<=threshold)
            {
                if (inputTable[i].awnser==1) counter_PrePos++;
                else counter_PreFal++;
            }
            else
            {
                if (inputTable[i].awnser==1) counter_PosPos++;
                else counter_PosFal++;
            }
        }

        //Calcular GINI pre e pos;
        gini_Pre = 1-pow(counter_PrePos/(counter_PreFal+counter_PrePos),2)-pow(counter_PreFal/(counter_PreFal+counter_PrePos),2);
        gini_Pos = 1-pow((counter_PosPos/(counter_PosFal+counter_PosPos)),2)-pow(counter_PosFal/(counter_PosFal+counter_PosPos),2);
        gini_Final= (gini_Pre*(counter_PreFal+counter_PrePos)+gini_Pos*(counter_PosFal+counter_PosPos))/lenghtOf_Table;

        is_Best=1;       
    }

    else
    {
        //Calcular impureza de best_Threshold, reutilizarei counter_pre e pos para isso
        for (int i=0;i<lenghtOf_Table;i++)
        {
            if (inputTable[i].value<=best_Threshold)
            {
                if (inputTable[i].awnser==1) counter_PrePos++;
                else counter_PreFal++;
            }
            else
            {
                if (inputTable[i].awnser==1) counter_PosPos++;
                else counter_PosFal++;
            }
        }
        //Calcular GINI pre e pos do best;
        gini_Pre = 1-pow(counter_PrePos/(counter_PreFal+counter_PrePos),2)-pow(counter_PreFal/(counter_PreFal+counter_PrePos),2);
        gini_Pos = 1-pow((counter_PosPos/(counter_PosFal+counter_PosPos)),2)-pow(counter_PosFal/(counter_PosFal+counter_PosPos),2);
        gini_Best= (gini_Pre*(counter_PreFal+counter_PrePos)+gini_Pos*(counter_PosFal+counter_PosPos))/lenghtOf_Table;

        //isPositive sera definido inicialmente como o isPositive do Best, caso o novo threshold seja melhor, sera substituido
        if (counter_PreFal>counter_PrePos){outputStruct.isPositive=1;}
        else {outputStruct.isPositive=0;}

        //Resetar contadores
        counter_PosFal=0; counter_PosPos=0; counter_PreFal=0; counter_PrePos=0;

        //Calcular impureza do novo threshold
        for (int i=0;i<lenghtOf_Table;i++)
        {
            if (inputTable[i].value<=threshold)
            {
                if (inputTable[i].awnser==1) counter_PrePos++;
                else counter_PreFal++;
            }
            else
            {
                if (inputTable[i].awnser==1) counter_PosPos++;
                else counter_PosFal++;
            }
        }
        gini_Pre = 1.0-pow(counter_PrePos/(counter_PreFal+counter_PrePos),2)-pow(counter_PreFal/(counter_PreFal+counter_PrePos),2);
        gini_Pos = 1.0-pow((counter_PosPos/(counter_PosFal+counter_PosPos)),2)-pow(counter_PosFal/(counter_PosFal+counter_PosPos),2);
        gini_Final= (gini_Pre*(counter_PreFal+counter_PrePos)+gini_Pos*(counter_PosFal+counter_PosPos))/lenghtOf_Table;

        //Se o novo gini for menor que o melhor, is_Best é 1;
        if (gini_Final<gini_Best)
        {
            is_Best=1;
        }
        else {is_Best=0;}
    }

    //Se for melhor que threshold anterior, substituir valores, se não, manter.
    if (is_Best==1)
    {
        outputStruct.feature_ID = ID;
        outputStruct.threshold = threshold;
        outputStruct.impurity = gini_Final;

        //Rodamos o .isPositive aqui pois os counter contem o valor do THR novo, assim substituindo o valor dado por best_THR
        if (counter_PreFal>counter_PrePos){outputStruct.isPositive=1;}
        else {outputStruct.isPositive=0;}
    }

    else
    {
        outputStruct.feature_ID = ID;
        outputStruct.threshold = best_Threshold;
        outputStruct.impurity = gini_Best;
    }

    return outputStruct;
}

//Vou usar isso para alocar vetores int
int* da_IntArr (int* variable, int size)
{
    variable = (int*)malloc(size*sizeof(int));
    if(variable == NULL)
    {
        perror("--Erro alocacao da_IntArr");
        getchar();
        exit(1);
    }

    return variable;
}
//Aloca uma matriz de int
int** da_Intmtx (int** variable, int rows, int colunms)
{
    variable = (int**)malloc(rows*sizeof(int*));
    if (variable==NULL)
    {
        perror("\n--da_Intmtx");
        getchar();
        exit(1);
    }
    
    for(int i=0;i<rows;i++)
    {
        variable[i]=(int*)malloc(colunms*sizeof(int));

        if(variable[i]==NULL)
        {
            perror("\n--da_Intmtx[i]");
            getchar();
            exit(1);
        }
    }
    return variable;
}

//Vou usar isso para alcoar vetores double
double* da_DBLArr (double* variable, int size)
{
    variable = (double*)malloc(size*sizeof(double));
    if(variable == NULL)
    {
        printf("--Erro alocação da_IntArr");
        getchar();
        exit(1);
    }

    return variable;
}

//Aloca uma matriz de double
double** da_DBLmtx (double** variable, int rows, int colunms)
{
    variable = (double**)malloc(rows*sizeof(double*));
    if (variable==NULL)
    {
        printf("--Erro da_DBLmtx");
        getchar();
        exit(1);
    }
    
    for(int i=0;i<rows;i++)
    {
        variable[i]=(double*)malloc(colunms*sizeof(double));

        if(variable[i]==NULL)
        {
            printf("--Erro da_DBLmtx[i]");
            getchar();
            exit(1);
        }
    }
    return variable;
}

//Desaloca uma matriz
void double_SuperFree(double** array, int rows)
{
        //Para todas as colunas, desalocar
        for (int i=0;i<rows;i++)
        {
            free(array[i]);
        }

    //Desalocar linhas
    free(array);
}

//Funcção que ordena o struct ordemFeature de acordo com gini crescente
struct ordemFeature sortByGini(struct ordemFeature input, int lineNumber, int featurenumber)
{
    struct ordemFeature localStruct;
    int* IDs_Excluidos = NULL;
    double menor_Gini = 2; //Gini não pode ser maior q 1
    int ID_Menor=0;
    double threshold_Menor=0;
    int isPositive_Menor=0;
    int isLeaf_Menor=0;

    //Inicialização de vetores
    localStruct.gini = da_DBLArr(localStruct.gini,featurenumber-2);
    localStruct.threshold = da_DBLArr(localStruct.threshold,featurenumber-2);
    localStruct.ID = da_IntArr(localStruct.ID,featurenumber-2);
    localStruct.isLeaf = da_IntArr(localStruct.isLeaf,featurenumber-2);
    localStruct.isPositive = da_IntArr(localStruct.isPositive,featurenumber-2);

    IDs_Excluidos = da_IntArr(IDs_Excluidos, featurenumber-2);
    //Preencher matriz com ID impossiveis
    for(int i=0;i<featurenumber-2;i++)
    {
        IDs_Excluidos[i]=-1;
    }
    
    //Para toda feature
    for (int i=0; i<featurenumber-2;i++)
    {
        menor_Gini=2;
        //Compara todas as feautures e acha aquela com menor gini e com ID não excluido
        for(int j=0; j<featurenumber-2;j++)
        {
            if(input.gini[j]<menor_Gini&&checkRepeat(IDs_Excluidos,input.ID[j], featurenumber)!=1)
            {
                //Anota os dados do menor gini atual
                ID_Menor = input.ID[j];
                menor_Gini = input.gini[j];
                isLeaf_Menor = input.isLeaf[j];
                isPositive_Menor = input.isPositive[j];
                threshold_Menor = input.threshold[j];
            }
        }
        //Adiciona a feature na primeria entrada junto com suas caracteristicas, e adiciona ID aos excluidos
        IDs_Excluidos[i] = ID_Menor;

        localStruct.gini[i] = menor_Gini;
        localStruct.ID[i] = ID_Menor;
        localStruct.isLeaf[i] = isLeaf_Menor;
        localStruct.isPositive[i] = isPositive_Menor;
        localStruct.threshold[i] = threshold_Menor;
    }

    return localStruct;
}

//É aqui
//A fronteira final
//A desilusão dos testes
int testStructure(struct Tree tree, double* testData, int featureNumber)
{
    int anwsered=0;
    int questionID=0;
    int featureRequested;
    double currentValue;
    int anwser=0;
    int currentNode=0;

    //printf("\n--Valor real e %d\n", (int)testData[featureNumber-1]);

    while (anwsered==0)
    {
        featureRequested = tree.galhos[currentNode].test_ID;

        //Isto é porquice para não cagar tudo
        if (currentNode>=NODE_LIMIT-1){
            return -1;
        }
        currentValue = testData[featureRequested];
        //printf("--- Current value:%f\tthreshold,isPos:%f, %d\n",currentValue,tree.galhos[currentNode].threshold,tree.galhos[currentNode].isPositive);

        //Testa se ele cai antes ou depois do limite
        if (currentValue<tree.galhos[currentNode].threshold)
        {
            //Se for pré, verifcar se for folha, se não, ir para proximo node
            if (tree.galhos[currentNode].left_Node==-1&&tree.galhos[currentNode].right_Node==-1)
            {
                //Se isPos=0, significa que menor que threshold no galho é positivo
                if (tree.galhos[currentNode].isPositive==0)
                {
                    anwser=1;
                    anwsered=1;
                }
                else
                {
                    anwser=0;
                    anwsered=1;
                }
            }
            else
            {
                currentNode=tree.galhos[currentNode].left_Node;
            }
        }

        //Se for maior que limite cai aqui
        else
        {
            //verificar se é folha
            if (tree.galhos[currentNode].left_Node==-1&&tree.galhos[currentNode].right_Node==-1)
            {
                //Se isPos=0, resposta para limite menor q valor é negativo
                if (tree.galhos[currentNode].isPositive==0)
                {
                    anwser=0;
                    anwsered=1;
                }
                else
                {
                    anwser=1;
                    anwsered=1;
                }
            }
            else
            {
                currentNode=tree.galhos[currentNode].right_Node;
            }
        }

    }

    return anwser;
}

//Cria um lista aleatoria de tamanho e numero limite especificado
int* rndList(int sizeof_List, int maxNumber, int minNumber)
{
    int randomNumber;
    int* outputList=NULL;
    int iterator=0;

    outputList = da_IntArr(outputList, sizeof_List);

    while (iterator<sizeof_List)
    {
        //Numero entre minNumber e maxNumber
        randomNumber = rand()%maxNumber + minNumber;

        //Se o numero não estiver na lista, adiciona-o e incrementa iterator
        if (checkRepeat(outputList,randomNumber,iterator)!=1)
        {
            outputList[iterator]=randomNumber;
            iterator++;
        }
    }

    return outputList;
}

//Constroi uma arvore
struct Tree buildTree(double** dataMatrix, int* sample_List, int* feature_List, int feature_Number, int sample_Number)
{
    struct valueAnswer* orderedList;
    TNode* node;
    struct giniOutput giniStruct;
    struct Tree output;

    int currentNode=0;
    int nodeCount=0;
    int features_Left = FEATURES_PER_TREE;
    int outOf_Samples =0;
    int bestFeature;
    int isPositive;
    double final_Threshold;

    double threshold;
    double bestThreshold;
    double bestGini=2;      //2 é um bom valor de inicio, ja que gini vai até 1(?)

    node = (TNode*)malloc(NODE_LIMIT*sizeof(TNode));

    //Eu não sei se é possivel prever quantidade de nos de uma arvore, mas sei que seu tamnhop maximo é a quantidade de features. Eu acho
    //Isso vai armazenar as samples e features de cada nó e quantidade de samples por linha , e inicia a do primeiro
    int** samples_Matrix = da_Intmtx(samples_Matrix,NODE_LIMIT+FEATURES_PER_TREE,NODE_LIMIT+FEATURES_PER_TREE);
    int* sample_Amount = da_IntArr(sample_Amount,NODE_LIMIT+FEATURES_PER_TREE);
    sample_Amount[0] = SAMPLE_SIZE;
    for (int i=0;i<SAMPLE_SIZE;i++)
    {
        samples_Matrix[0][i]=sample_List[i];
    }

    //Zera o valor de sample_Amount
    for (int i=1;i<FEATURES_PER_TREE;i++)
    {
        sample_Amount[i]=0;
    }

    //Enquando eu não mandar a função parar (cada iteração do while é um nó)
    while (outOf_Samples==0)
    {
        //Resetar melhor gini para cada nó
        bestGini=2;

        //Para toda feature na lista de features selecionadas
        for(int i=0;i<FEATURES_PER_TREE;i++)
        {
            bestThreshold=-1;

            //Ordenar de forma crescente
            orderedList = sortFeature(dataMatrix,feature_List[i],samples_Matrix[currentNode],sample_Amount[currentNode],feature_Number);
            
            //Para todo threshold (Em uma lista, temos numero de samples do nó-1 thresholds possiveis)
            for (int j=0 ;j<sample_Amount[currentNode]-1;j++)
            {
                //Se dois valores seguidos da dataMatrix no ponto sampleMatrix na linha currentNode e feature atual forem iguais, não rodar
                if (dataMatrix[samples_Matrix[currentNode][j]][feature_List[i]]==dataMatrix[samples_Matrix[currentNode][j+1]][feature_List[i]]){}
                else 
                {
                    threshold = (orderedList[j].value+orderedList[j+1].value)/2;
                    giniStruct = calcGini(feature_List[i],threshold,bestThreshold,orderedList,sample_Amount[currentNode]);
                    bestThreshold = giniStruct.threshold;
                }
            }
            //Se a nova impuridade for a melhor até agora, anotar suas informações
            if (giniStruct.impurity<bestGini)
            {
                bestFeature = giniStruct.feature_ID;
                bestGini = giniStruct.impurity;
                isPositive = giniStruct.isPositive;
                final_Threshold = giniStruct.threshold;
            }
        }
        //Neste ponto, para um dado nó, ja foi encontrada sua melhor feature, agora, vamos criar o nó
        node[currentNode].gini = bestGini;
        node[currentNode].isPositive = isPositive;
        node[currentNode].test_ID = bestFeature;
        node[currentNode].threshold = final_Threshold;
        node[currentNode].left_Node = nodeCount+1;
        node[currentNode].right_Node = nodeCount+2;

        //Se o nó tiver muitas poucas samples, ele não deve criar outros pors é uma folha, demarcado por seus nós adjacentes serem -1
        if (sample_Amount[currentNode]<MIN_SAMPLES_PER_NODE)
        {
            printf("\n\n--sample_Amount[currentNode]=%d",sample_Amount[currentNode]);
            node[currentNode].left_Node = -1;
            node[currentNode].right_Node = -1;
            printf("\n--O no %d e uma folha isPostive(%d)!",currentNode,isPositive);
        }

        else
        {
            // Precisamos dividir o sampleList e atualizar o sample_Matrix
            // Para todos os samples do nó atual, para isso precisaremos de contadores para manter a posição de preenchimento da lista pré e pos
            int position_pre = 0;
            int position_pos = 0;
            printf("\n\n--sample_Amount[currentNode]=%d",sample_Amount[currentNode]);
            for (int i = 0; i < sample_Amount[currentNode]; i++)
            {
                // Se na dataMatrix na linha samplesMatrix atual coluna best feature
                // Se menor vai para a node pre
                //printf("\n--dataMatrix[samples_Matrix[currentNode][i]][bestFeature]=%f",dataMatrix[samples_Matrix[currentNode][i]][bestFeature]);
                if (dataMatrix[samples_Matrix[currentNode][i]][bestFeature] < final_Threshold)
                {
                    // Prencher proxima posição, muda proxima posição e aumenta o contador de quantidade de samples de um nó
                    samples_Matrix[nodeCount + 1][position_pre] = samples_Matrix[currentNode][i];
                    position_pre++;
                    sample_Amount[nodeCount + 1]++;
                }
                //Checa se

                // Se maior
                else
                {
                    // place into the POS list (use position_pos)
                    samples_Matrix[nodeCount + 2][position_pos] = samples_Matrix[currentNode][i];
                    position_pos++;
                    sample_Amount[nodeCount + 2]++;
                }
            }

            // Como criamos duas novas nodes, adicionamos 2 ao contador
            // advance nodeCount to the next free node indices
            nodeCount += 2;
        }
        //Proxima Node
        printf("\n\n----------------------------------------\nNode %d concluida, indo para Node%d\n",currentNode,currentNode+1);
        currentNode++;
        //Se o nó atual é maior que a qauntidade de nós, signfica que a arvre esta completa
        if (currentNode>nodeCount||nodeCount>NODE_LIMIT)
        {
            outOf_Samples=1;
        }
    }
    output.galhos = node;
    return output;
}

struct valueAnswer* sortFeature(double** dataMatrix, int feature_ID, int* sample_List, int sample_amount, int feature_Number)
{
    int* IDs_Excluidos = da_IntArr(IDs_Excluidos,sample_amount);
    double menor_Valor=10.1E10;
    int menor_ID=0;

    //Cria uma lista de resposta de tamanho sample_amount
    struct valueAnswer* outPut = (struct valueAnswer*)malloc(sample_amount*sizeof(struct valueAnswer));
    if (outPut==NULL)
    {
        perror("\n--sortFeature AL:");
        getchar();
        exit(1);
    }

    //Preenche IDs_Excluidos com valores impossiveis
    for (int i=0;i<sample_amount;i++)
    {
        IDs_Excluidos[i]=-1;
    }

    //Enquato output não estiver cheio
    for (int i=0; i<sample_amount; i++)
    {
        menor_Valor = 10.1E10; //Resetando o valor
        //para todas as samples
        for(int j=0;j<sample_amount;j++)
        {
            //Se for o menor (e não excluido) valor anotar o valor e o ID
            if (dataMatrix[sample_List[j]][feature_ID]<menor_Valor&&(checkRepeat(IDs_Excluidos,sample_List[j],i)!=1))
            {
                menor_Valor = dataMatrix[sample_List[j]][feature_ID];
                menor_ID = sample_List[j];
            }
        }
        //Anota menor valor e ID e o excloi ID

        outPut[i].awnser=dataMatrix[menor_ID][feature_Number-1];
        outPut[i].value=menor_Valor;
        IDs_Excluidos[i]=menor_ID;
    }
    free(IDs_Excluidos);
    return outPut;
}