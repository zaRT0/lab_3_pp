import matplotlib.pyplot as plt

def plot_multiplication_time_comparison():
    """
    Строит графики времени умножения матриц для разного количества потоков.
    """
    data = {
        'Threads: 1': {
            'sizes': [100, 200, 500, 750, 1000, 1300, 1500, 1700, 2000],
            'times': [0.001, 0.003, 0.065, 0.294, 0.688, 2.013, 3.27, 5.955, 12.306]
        },
        'Threads: 2': {
            'sizes': [100, 200, 500, 750, 1000, 1300, 1500, 1700, 2000],
            'times': [0.0006, 0.001, 0.033, 0.134, 0.411, 1.191, 1.902, 3.0, 6.675]
        },
        'Threads: 4': {
            'sizes': [100, 200, 500, 750, 1000, 1300, 1500, 1700, 2000],
            'times': [0.0003, 0.001, 0.018, 0.084, 0.241, 0.751, 1.212, 2.01, 4.059]
        },
        'Threads: 6': {
            'sizes': [100, 200, 500, 750, 1000, 1300, 1500, 1700, 2000],
            'times': [0.0002, 0.0008, 0.013, 0.075, 0.238, 0.692, 1.062, 1.824, 3.443]
        },
        'Threads: 8': {
            'sizes': [100, 200, 500, 750, 1000, 1300, 1500, 1700, 2000],
            'times': [0.0001, 0.001, 0.012, 0.075, 0.204, 0.657, 1.207, 1.829, 3.44]
        },
        'Threads: 10': {
            'sizes': [100, 200, 500, 750, 1000, 1300, 1500, 1700, 2000],
            'times': [0.0001, 0.0007, 0.019, 0.073, 0.206, 0.684, 1.366, 1.806, 3.402]
        }
    }

    plt.figure(figsize=(10, 6))
    
    colors = ['b', 'g', 'r', 'c', 'm', 'y']
    
    for i, (label, values) in enumerate(data.items()):
        plt.plot(values['sizes'], values['times'], 
                marker='o', linestyle='-', 
                color=colors[i], 
                linewidth=2,
                markersize=8,
                label=label)
    
    plt.xlabel('Размер матрицы (N x N)', fontsize=12)
    plt.ylabel('Время (секунды)', fontsize=12)
    plt.title('Зависимость времени умножения матриц от размера', fontsize=14)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend(fontsize=10)
    
    plt.xscale('log')
    plt.yscale('log')
    
    plt.xticks([100, 200, 500, 750, 1000, 1300, 1500, 1700, 2000],
               ['100', '200', '500', '750', '1000', '1300', '1500', '1700', '2000'])
    
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    plot_multiplication_time_comparison()