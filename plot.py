import pandas as pd
import matplotlib.pyplot as plt


def plot_metric(data: pd.DataFrame, dataset: str, metric: str, ylabel: str) -> None:
    subset = data[data["dataset"] == dataset]

    plt.figure(figsize=(12, 7))

    for algorithm in sorted(subset["algorithm"].unique()):
        algorithm_data = subset[subset["algorithm"] == algorithm]
        plt.plot(
            algorithm_data["size"],
            algorithm_data[metric],
            marker="o",
            linewidth=1.5,
            label=algorithm,
        )

    plt.title(f"{dataset}: {ylabel}")
    plt.xlabel("Количество строк")
    plt.ylabel(ylabel)
    plt.grid(True)
    plt.legend()
    plt.tight_layout()

    filename = f"{dataset}_{metric}.png"
    plt.savefig(filename, dpi=200)
    plt.close()

    print(f"saved: {filename}")


def main() -> None:
    data = pd.read_csv("results.csv")

    for dataset in sorted(data["dataset"].unique()):
        plot_metric(data, dataset, "avg_time_us", "Среднее время, мкс")
        plot_metric(
            data,
            dataset,
            "avg_symbol_operations",
            "Среднее количество посимвольных операций",
        )


if __name__ == "__main__":
    main()
