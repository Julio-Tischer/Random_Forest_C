import pandas as pd
from sklearn.model_selection import train_test_split

try:
    df = pd.read_csv('seus_dados.csv')
except FileNotFoundError:
    print("="*50)
    print("ERRO: Arquivo 'seus_dados.csv' não encontrado!")
    print("Por favor, crie este arquivo na mesma pasta do script.")
    print("="*50)
    exit() 
    # Sai do script se o arquivo não existe

print("--- Dados Originais Carregados (primeiras 5 linhas) ---")
print(df.head())

# separe as Features (X) e (y) 

target_column = 'class'
y = df[target_column]

X = df.drop(columns=[target_column, 'id'])


X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.2, random_state=42
)
# verificar resultados
print("\n--- Alvo (y) (primeiras 5 linhas) ---")
print(y.head())

print("\n--- Features (X) (primeiras 5 linhas) ---")
print(X.head())

print("\n--- Divisão dos dados concluída ---")
print(f"Total de amostras: {len(df)}")
print(f"Amostras de Treino: {len(X_train)} (80%)")
print(f"Amostras de Teste:   {len(X_test)} (20%)")

# Recombine features + target and salvar como CSV sem cabeçalho e sem coluna 'id'
try:
    train_df = X_train.copy()
    train_df[target_column] = y_train.values

    test_df = X_test.copy()
    test_df[target_column] = y_test.values

    # Garantir que não exista coluna 'id' (por segurança)
    train_df = train_df.drop(columns=['id'], errors='ignore')
    test_df = test_df.drop(columns=['id'], errors='ignore')

    # Salvar sem cabeçalho e sem índice
    train_df.to_csv('train.csv', index=False, header=False)
    test_df.to_csv('test.csv', index=False, header=False)

    print("\n--- Arquivos gerados: train.csv e test.csv (sem cabeçalho, sem coluna id) ---")
except Exception as e:
    print("Erro ao salvar os arquivos de saída:", e)