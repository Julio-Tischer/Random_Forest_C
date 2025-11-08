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

X = df.drop(columns=[target_column, 'ID'])


X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.3, random_state=42
)
# verificar resultados
print("\n--- Alvo (y) (primeiras 5 linhas) ---")
print(y.head())

print("\n--- Features (X) (primeiras 5 linhas) ---")
print(X.head())

print("\n--- Divisão dos dados concluída ---")
print(f"Total de amostras: {len(df)}")
print(f"Amostras de Treino: {len(X_train)} (70%)")
print(f"Amostras de Teste:   {len(X_test)} (30%)")