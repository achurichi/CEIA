from pathlib import Path
import logging
import numpy as np
import os
import pickle


class WordsEmbeddings(object):
    logger = logging.getLogger(__name__)

    def __init__(self):
        # load the embeddings
        words_embedding_pkl = Path(self.pkl_path)
        if not words_embedding_pkl.is_file():
            words_embedding_txt = Path(self.word_to_vec_model_txt_path)
            assert words_embedding_txt.is_file(), "Words embedding not available"
            embeddings = self.convert_model_to_pickle()
        else:
            embeddings = self.load_model_from_pickle()
        self.embeddings = embeddings
        # build the vocabulary hashmap
        index = np.arange(self.embeddings.shape[0])
        # Dicctionarios para traducir de embedding a IDX de la palabra
        self.word2idx = dict(zip(self.embeddings["word"], index))
        self.idx2word = dict(zip(index, self.embeddings["word"]))

    def get_words_embeddings(self, words):
        words_idxs = self.words2idxs(words)
        return self.embeddings[words_idxs]["embedding"]

    def words2idxs(self, words):
        return np.array([self.word2idx.get(word, -1) for word in words])

    def idxs2words(self, idxs):
        return np.array([self.idx2word.get(idx, "-1") for idx in idxs])

    def load_model_from_pickle(self):
        self.logger.debug(
            "loading words embeddings from pickle {}".format(self.pkl_path)
        )
        max_bytes = 2**28 - 1  # 256MB
        bytes_in = bytearray(0)
        input_size = os.path.getsize(self.pkl_path)
        with open(self.pkl_path, "rb") as f_in:
            for _ in range(0, input_size, max_bytes):
                bytes_in += f_in.read(max_bytes)
        embeddings = pickle.loads(bytes_in)
        self.logger.debug("words embeddings loaded")
        return embeddings

    def convert_model_to_pickle(self):
        # create a numpy strctured array:
        # word     embedding
        # U50      np.float32[]
        # word_1   a, b, c
        # word_2   d, e, f
        # ...
        # word_n   g, h, i
        self.logger.debug(
            "converting and loading words embeddings from text file {}".format(
                self.word_to_vec_model_txt_path
            )
        )
        structure = [
            ("word", np.dtype("U" + str(self.word_max_size))),
            ("embedding", np.float32, (self.n_features,)),
        ]
        structure = np.dtype(structure)
        # load numpy array from disk using a generator
        with open(
            self.word_to_vec_model_txt_path, encoding="utf8"
        ) as words_embeddings_txt:
            embeddings_gen = (
                (line.split()[0], line.split()[1:])
                for line in words_embeddings_txt
                if len(line.split()[1:]) == self.n_features
            )
            embeddings = np.fromiter(embeddings_gen, structure)
        # add a null embedding
        null_embedding = np.array(
            [("null_embedding", np.zeros((self.n_features,), dtype=np.float32))],
            dtype=structure,
        )
        embeddings = np.concatenate([embeddings, null_embedding])
        # dump numpy array to disk using pickle
        max_bytes = 2**28 - 1  # # 256MB
        bytes_out = pickle.dumps(embeddings, protocol=pickle.HIGHEST_PROTOCOL)
        with open(self.pkl_path, "wb") as f_out:
            for idx in range(0, len(bytes_out), max_bytes):
                f_out.write(bytes_out[idx : idx + max_bytes])
        self.logger.debug("words embeddings loaded")
        return embeddings


class GloveEmbeddings(WordsEmbeddings):
    def __init__(
        self,
        word_to_vec_model_txt_path="glove.twitter.27B.50d.txt",
        pkl_path="gloveembedding.pkl",
        n_features=50,
        word_max_size=60,
    ):
        self.word_to_vec_model_txt_path = word_to_vec_model_txt_path
        self.pkl_path = pkl_path
        self.n_features = n_features
        self.word_max_size = word_max_size
        super().__init__()


class FasttextEmbeddings(WordsEmbeddings):
    def __init__(
        self,
        word_to_vec_model_txt_path="cc.en.300.vec",
        pkl_path="fasttext.pkl",
        n_features=300,
        word_max_size=60,
    ):
        self.word_to_vec_model_txt_path = word_to_vec_model_txt_path
        self.pkl_path = pkl_path
        self.n_features = n_features
        self.word_max_size = word_max_size
        super().__init__()
