import pandas as pd
from sklearn.model_selection import train_test_split
import numpy as np
import random

def create_synthetic_data(num_samples=100):
    np.random.seed(42)
    random.seed(42)
    
    data = {
        'feature_1': np.random.rand(num_samples) * 10,
        'feature_2': np.random.randint(50, 100, num_samples),
        'feature_3': np.random.normal(loc=10.0, scale=2.0, size=num_samples),
        'feature_4': np.random.uniform(low=0.1, high=1.0, size=num_samples),
        'feature_5': np.random.poisson(lam=3, size=num_samples)
    }
    
    df = pd.DataFrame(data)
    
    df['id'] = range(1, num_samples + 1)
    
    classes = ['A', 'B', 'C']
    df['class'] = [random.choice(classes) for _ in range(num_samples)]
    
    print("="*50)
    print(f"ATENÇÃO: Arquivo 'seus_dados.csv' não encontrado. Gerando {num_samples} dados sintéticos.")
    print("="*50)
    return df

try:
    df = pd.read_csv('seus_dados.csv')
except FileNotFoundError:
    df = create_synthetic_data(num_samples=100)
    
print("--- Dados Originais Carregados (primeiras 5 linhas) ---")
print(df.head())

target_column = 'class'
y = df[target_column]

X = df.drop(columns=[target_column, 'id'])

X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.5, random_state=42
)

print("\n--- Alvo (y) (primeiras 5 linhas) ---")
print(y.head())

print("\n--- Features (X) (primeiras 5 linhas) ---")
print(X.head())

print("\n--- Divisão dos dados concluída ---")
print(f"Total de amostras: {len(df)}")
print(f"Amostras de Treino: {len(X_train)} (50%)")
print(f"Amostras de Teste:   {len(X_test)} (50%)")

try:
    train_df = X_train.copy()
    train_df[target_column] = y_train.values

    test_df = X_test.copy()
    test_df[target_column] = y_test.values

    train_df = train_df.drop(columns=['id'], errors='ignore')
    test_df = test_df.drop(columns=['id'], errors='ignore')

    train_df.to_csv('train.csv', index=False, header=False)
    test_df.to_csv('test.csv', index=False, header=False)

    print("\n--- Arquivos gerados: train.csv e test.csv (sem cabeçalho, sem coluna id) ---")
except Exception as e:
    print("Erro ao salvar os arquivos de saída:", e)