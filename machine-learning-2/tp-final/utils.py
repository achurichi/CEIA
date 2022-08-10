from sklearn.metrics import (
    balanced_accuracy_score,
    roc_auc_score,
    precision_score,
    recall_score,
)


def report_scores(y_test, y_pred, y_proba):
    message = [
        f"Scores Report:",
        f"AUC ROC: {roc_auc_score(y_test, y_proba[:, 1]):.4f}",
        f"Balanced Accuracy: {balanced_accuracy_score(y_test, y_pred):.4f}",
        f"Precision: {precision_score(y_test, y_pred):.4f}",
        f"Recall: {recall_score(y_test, y_pred):.4f}",
    ]
    message = "\n".join(message)

    print(message)
