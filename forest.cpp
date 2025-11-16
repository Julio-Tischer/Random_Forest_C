
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
#define TREE_NUMBER 22                   //Quantas arvores a serem feitas (+ = +tempo)
#define SAMPLE_SIZE 297                  //Quantas samples por arvore (+ = +tempo)
#define FEATURES_PER_TREE 122            //Quantas features por arvore(+ = +tempo)
#define MIN_SAMPLES_PER_NODE 59          //Minimo da samples para um nó ser considerado uma folha(+ = -tempo)
#define NODE_LIMIT 50                    //Limite de nós de uma arvore, não diminui o tempo de processamento AO MENOS que seja um valor muito baixo (<15), mas aumenta qtd de arvores invalidadas

#define VERBOSE 0                         //Eu peguei essa ideia do copilot, defina como 0 para diminuir os prints
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
    FILE* FILE_Metrics = NULL;

    int numeroFeatures=0;
    int numeroLinhas=0;
    int testeLinhas=0;

    //Variaveis de teste
    int TP=0;
    int TN=0;
    int FP=0;
    int FN=0;
    double err = (double)(FP+FN)/(FP+FN+TP+TN);
    double acc = (double)(TP+TN)/(FP+FN+TP+TN);
    double pre = (double)(TP)/(TP+FP);
    double rec = (double)(TP)/(TP+FN);
    double F1 = (2*pre*rec)/(pre+rec);

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
        if (resposta==testTable[i][numeroFeatures-1] && testTable[i][numeroFeatures-1]==1)
        {
            TP++;
        }
        else if (resposta==testTable[i][numeroFeatures-1] && testTable[i][numeroFeatures-1]==0)
        {
            TN++;
        }
        else if (resposta!=testTable[i][numeroFeatures-1] && resposta==1)
        {
            FP++;
        }
        else
        {
            FN++;
        }
    }
    err = (double)(FP+FN)/(FP+FN+TP+TN);
    acc = (double)(TP+TN)/(FP+FN+TP+TN);
    pre = (double)(TP)/(TP+FP);
    rec = (double)(TP)/(TP+FN);
    F1 = (2*pre*rec)/(pre+rec);

    printf("\n\n TP:%d\t| FN:%d\n-----------------\n FP:%d\t| TN:%d\n",TP,FN,FP,TN);
    printf("\nPrediction Error:\t%f\n",err);
    printf("\nAccuracy:\t\t%f\t---%f%%\n",acc,acc*100);
    printf("\nPrecision:\t\t%f\t---%f%%\n",pre,pre*100);
    printf("\nRecall:\t\t\t%f\n",rec);
    printf("\nF1_Score:\t\t%f\n",F1);

    FILE_Metrics = fopen("metrics.csv","a");
    fprintf(FILE_Metrics,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f,%f,%f\n",TREE_NUMBER,SAMPLE_SIZE,FEATURES_PER_TREE,MIN_SAMPLES_PER_NODE,NODE_LIMIT,TP,TN,FP,FN,err,acc,pre,rec,F1);

    for(int i = 0; i < TREE_NUMBER; i++) {
        free(forest[i].galhos);
    }
    free(forest);
    //---------------------------------------------------------------------------------------------//

    //Encerrção do codigo
    fclose(FILE_Train);
    fclose(FILE_test);
    fclose(FILE_Metrics);

    double_SuperFree(dataTable,numeroLinhas);
    double_SuperFree(testTable,testeLinhas);
    
    //para terminal não fechar sozinho
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
        // isPos em 2 significa que qualquer valor do nó é positivo, -1 que qualquer valor do no é negativo
        //Se a pre negativo e pos positivo
        if (counter_PreFal>counter_PrePos && counter_PosFal<=counter_PosPos)
        {
            outputStruct.isPositive=1;
        }
        //Se pre positivo e pos negativo
        else if (counter_PreFal<=counter_PrePos && counter_PosFal>counter_PosPos)
        {
            outputStruct.isPositive=0;
        }
        //Se pre E pos negativos
        else if(counter_PreFal>counter_PrePos && counter_PosFal>counter_PosPos)
        {
            outputStruct.isPositive=-1;
        }
        //Se pre E pos positivos
        else if(counter_PreFal<=counter_PrePos && counter_PosFal<=counter_PosPos)
        {
            outputStruct.isPositive=2;
        }
        else 
        {
            printf("\n\nOh fiddlesticks, what now?");
            getchar();
            exit(1);
        }
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
        if (currentNode>=NODE_LIMIT-3||(featureRequested<0||featureRequested>featureNumber)){
            return -1;
        }
        currentValue = testData[featureRequested];

        //Primeiro analise se a folha é garantida POS ou garantida NEG
        if (tree.galhos[currentNode].isPositive==2)
        {
            anwser=1;
            anwsered=1;
        }

        else if (tree.galhos[currentNode].isPositive==-1)
        {
            anwser=0;
            anwsered=1;
        }

        //Testa se ele cai antes ou depois do limite
        else if (currentValue<tree.galhos[currentNode].threshold)
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
    struct valueAnswer* orderedList = NULL;
    TNode* node;
    struct giniOutput giniStruct;
    struct Tree output;

    int currentNode=0;
    int nodeCount=0;
    int features_Left = FEATURES_PER_TREE;
    int outOf_Samples =0;
    int bestFeature;
    int isPositive;
    int* usedFeatures;
    double final_Threshold;

    double threshold;
    double bestThreshold;
    double bestGini=2;      //2 é um bom valor de inicio, ja que gini vai até 1(?)

    node = (TNode*)malloc(NODE_LIMIT*sizeof(TNode));

    //Essa é uma lista que talvez resolva o problema de arvores invalidas, mas o ideal seria trabalhar com uma matriz parecida com samples_Matriz
    usedFeatures = da_IntArr(usedFeatures,NODE_LIMIT+1);
    for (int i=0;i<NODE_LIMIT+1;i++)
    {
        usedFeatures[i]=-1;
    }

    //Eu não sei se é possivel prever quantidade de nos de uma arvore, mas sei que seu tamnhop maximo é a quantidade de features. Eu acho
    //Isso vai armazenar as samples e features de cada nó e quantidade de samples por linha , e inicia a do primeiro
    int** samples_Matrix = da_Intmtx(samples_Matrix,NODE_LIMIT+FEATURES_PER_TREE,SAMPLE_SIZE);
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
            if (sample_Amount<0)
            {
                perror("");
            }
            orderedList = sortFeature(dataMatrix,feature_List[i],samples_Matrix[currentNode],sample_Amount[currentNode],feature_Number);
            if (orderedList==NULL){break;}
            
            //Para todo threshold (Em uma lista, temos numero de samples do nó-1 thresholds possiveis)
            for (int j=0 ;j<sample_Amount[currentNode]-1;j++)
            {
                //Se dois valores seguidos lista ordenada forem iguais, ignorar
                if (orderedList[j].value==orderedList[j+1].value){}
                else 
                {
                    threshold = (orderedList[j].value+orderedList[j+1].value)/2;
                    giniStruct = calcGini(feature_List[i],threshold,bestThreshold,orderedList,sample_Amount[currentNode]);
                    bestThreshold = giniStruct.threshold;
                    if (giniStruct.impurity==0)
                    {
                        break;
                    }
                }
            }
            //Se a nova impuridade for a melhor até agora, e a features não foi usada, anotar suas informações
            // && checkRepeat(usedFeatures,giniStruct.feature_ID,nodeCount)!=1
            if (giniStruct.impurity==0||(giniStruct.impurity<bestGini))
            {
                bestFeature = giniStruct.feature_ID;
                bestGini = giniStruct.impurity;
                isPositive = giniStruct.isPositive;
                final_Threshold = giniStruct.threshold;
            }
            if (bestGini==0)
            {
                break;
            }
            //Libererar orderedList antes de alocar a proxima
            free(orderedList);
        }
        //Neste ponto, para um dado nó, ja foi encontrada sua melhor feature, agora, vamos criar o nó
        node[currentNode].gini = bestGini;
        node[currentNode].isPositive = isPositive;
        node[currentNode].test_ID = bestFeature;
        node[currentNode].threshold = final_Threshold;
        node[currentNode].left_Node = nodeCount+1;
        node[currentNode].right_Node = nodeCount+2;
        usedFeatures[currentNode] = bestFeature;

        //Se o nó tiver muitas poucas samples, OU ele tiver impureza 0, ele não deve criar outros pois é uma folha, demarcado por seus nós adjacentes serem -1
        if (sample_Amount[currentNode]<MIN_SAMPLES_PER_NODE|| bestGini==0)
        {
            node[currentNode].left_Node = -1;
            node[currentNode].right_Node = -1;
        }

        else
        {
            // Precisamos dividir o sampleList e atualizar o sample_Matrix
            // Para todos os samples do nó atual, para isso precisaremos de contadores para manter a posição de preenchimento da lista pré e pos
            int position_pre = 0;
            int position_pos = 0;
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

        if (VERBOSE!=0){
            printf("\n\n----------------------------------------\nNode %d:\tImpureza:%f\tFeature:%d\tSamples:%d\n",currentNode,bestGini,bestFeature,sample_Amount[currentNode]);
            printf("No esquerdo:%d\tNo direito:%d\tIsPositive:%d\tThreshold:%f\n",node[currentNode].left_Node,node[currentNode].right_Node,node[currentNode].isPositive,node[currentNode].threshold);
        }
        currentNode++;
        //Se o nó atual é maior que a qauntidade de nós, signfica que a arvre esta completa
        if (currentNode>nodeCount||nodeCount>NODE_LIMIT)
        {
            outOf_Samples=1;
        }
    }
    output.galhos = node;

    //Liberar a sample_matrix e sample_amount e usedFeatures
    for(int i = 0; i < NODE_LIMIT + FEATURES_PER_TREE; i++)
    {
        free(samples_Matrix[i]);
    }
    free(samples_Matrix);
    free(sample_Amount);
    free(usedFeatures);
    return output;
}

struct valueAnswer* sortFeature(double** dataMatrix, int feature_ID, int* sample_List, int sample_amount, int feature_Number)
{
    //Cria uma lista de resposta de tamanho sample_amount
    struct valueAnswer* outPut = (struct valueAnswer*)malloc((sample_amount)*sizeof(struct valueAnswer));
    if (outPut==NULL)
    {perror("sortFeature NULLTST");getchar();return NULL;}

    //Cria burrfer de valueAnswer
    struct valueAnswer buffer;

    //Preencher a lista
    for (int i=0;i<sample_amount;i++)
    {
        outPut[i].value = dataMatrix[sample_List[i]][feature_ID];
        outPut[i].awnser = dataMatrix[sample_List[i]][feature_Number-1];
    }

    
    //Ordenar a lista com um Bubblesort ;(
    for (int i=0; i<sample_amount;i++)
    {
        for (int j=0;j<sample_amount-i-1;j++)
        {
            if (outPut[j].value>outPut[j+1].value)
            {
                buffer = outPut[j];
                outPut[j] = outPut[j+1];
                outPut[j+1] = buffer;
            }
        }
    }
    
    return outPut;
}