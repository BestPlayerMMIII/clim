from bitarray import bitarray
from typing import List, Dict, Tuple, Union, Callable

from Converter.encoder.utils import align_to_byte, huffman_coding


### HEADER CONSTRUCTION FUNCTIONS
def construct_huffman_header() -> bitarray:
    """Constructs the header for Huffman encoding."""
    return bitarray("0", endian="big")  # is_rle = False


def construct_rle_header(rle_bit_length: int) -> bitarray:
    """Constructs the header for RLE encoding."""
    header = bitarray("10", endian="big")  # is_rle = True, uses_huffman = False
    header.extend(format(rle_bit_length - 1, "05b"))  # Add 5-bit rle_bit_length
    return header


def construct_rle_huffman_header(rle_huffman_codebook: Dict[int, str]) -> bitarray:
    """Constructs the header for RLE + Huffman encoding."""
    header = bitarray("11", endian="big")  # is_rle = True, uses_huffman = True

    num_entries = len(rle_huffman_codebook)
    num_entries_bit_length = max(1, num_entries.bit_length())
    header.extend(format(num_entries_bit_length, "04b"))
    header.extend(format(num_entries, f"0{num_entries_bit_length}b"))

    max_value = max(rle_huffman_codebook.keys())
    max_value_bit_length = max(1, max_value.bit_length())
    header.extend(format(max_value_bit_length, "04b"))

    for value, huffman_code in rle_huffman_codebook.items():
        header.extend(format(value, f"0{max_value_bit_length}b"))
        header.extend(format(len(huffman_code) - 1, "04b"))
        header.extend(huffman_code)

    return header


### HELPER FUNCTIONS
def _generate_huffman_sequence(rle_indices: List[Tuple[int, int]], huffman_codebook: Dict[int, str]) -> bitarray:
    """Generates a Huffman-encoded sequence."""
    sequence = bitarray(endian="big")
    for palette_index, count in rle_indices:
        sequence.extend(huffman_codebook[palette_index] * count)
    return sequence


def _generate_rle_sequence(rle_indices: List[Tuple[int, int]], huffman_codebook: Dict[int, str], rle_bit_length: int) -> bitarray:
    """Generates an RLE-encoded sequence."""
    sequence = bitarray(endian="big")
    for index, count in rle_indices:
        sequence.extend(huffman_codebook[index])
        sequence.extend(format(count - 1, f"0{rle_bit_length}b"))
    return sequence


def _generate_rle_huffman_sequence(rle_indices: List[Tuple[int, int]], huffman_codebook: Dict[int, str], rle_huffman_codebook: Dict[int, str]) -> bitarray:
    """Generates an RLE + Huffman-encoded sequence."""
    sequence = bitarray(endian="big")
    for index, count in rle_indices:
        sequence.extend(huffman_codebook[index])
        sequence.extend(rle_huffman_codebook[count - 1])
    return sequence


### MAIN FUNCTION
def calculate_best_encoding(
    rle_indices: List[Tuple[int, int]],
    huffman_codebook: Dict[int, str],
    header_huffman_function: Callable[[], bitarray] = construct_huffman_header,
    header_rle_function: Callable[[int], bitarray] = construct_rle_header,
    header_rle_huffman_function: Callable[[Dict[int, str]], bitarray] = construct_rle_huffman_header,
    align_to_byte_header: bool = False,
    align_to_byte_sequence: bool = False,
    align_to_byte_all: bool = True
) -> Tuple[Dict[str, bytes], Dict[str, Union[str, bool, int, Dict[int, str]]]]:
    """
    Computes and selects the most efficient encoding method among Huffman, RLE, and RLE with Huffman.

    The function takes a sequence of RLE indices and uses the provided Huffman code to determine the optimal compression method. It compares the total size of the results obtained with the three methods
    (Huffman, RLE, and RLE with Huffman) and returns the one with the smallest data size.

    Arguments:
        - rle_indices (List[Tuple[int, int]]): List of tuples representing palette indices and their frequencies (palette_index, count).
        - huffman_codebook (Dict[int, str]): Dictionary that maps a palette index to its corresponding Huffman code.
        - header_huffman_function (Callable[[], bitarray], optional): Function to construct the header for Huffman encoding. Defaults to `construct_huffman_header`.
        - rle_header_function (Callable[[int], bitarray], optional): Function to construct the header for RLE encoding. Defaults to `construct_rle_header`.
        - header_rle_huffman_function (Callable[[Dict[int, str]], bitarray], optional): Function to construct the header for RLE encoding with Huffman. Defaults to `construct_rle_huffman_header`.
        - align_to_byte_header (bool, optional): If `True`, align the header to a byte by padding it. Defaults to `False`.
        - align_to_byte_sequence (bool, optional): If `True`, align the bit sequence to a byte by padding it. Defaults to `False`.
        - align_to_byte_all (bool, optional): If `True`, align the entire result (header + sequence) to a byte. Defaults to `True`.

    Returns:
    Tuple[Dict[str, byte], Dict[str, Union[str, bool, int, Dict[int, str]]]]:
    - The first element of the tuple is a dictionary with:
        - 'sequence': The compressed sequence in bytes format.
        - 'header': The compression header in bytes format.
        - 'all': The complete set (header + sequence) in bytes format.

    - The second element of the tuple is a dictionary containing details about the chosen method:
        - 'method': The method used ("Huffman", "RLE" or "RLE+Huffman").
        - 'is_rle': `True` if RLE was used, otherwise `False`.
        - 'uses_huffman': `True` if Huffman was used in combination with RLE.
        - 'rle_bit_length' (RLE only): The bit length to represent RLE values.
        - 'rle_huffman_codebook' (RLE+Huffman only): The RLE+Huffman codebook dictionary.
    """

    ### Huffman Encoding
    huffman_sequence = _generate_huffman_sequence(rle_indices, huffman_codebook)
    huffman_header = header_huffman_function()

    ### RLE Encoding
    rle_bit_length = max((count - 1).bit_length() for _, count in rle_indices)
    rle_sequence = _generate_rle_sequence(rle_indices, huffman_codebook, rle_bit_length)
    rle_header = header_rle_function(rle_bit_length)

    ### RLE + Huffman Encoding
    rle_huffman_codebook = huffman_coding({count - 1: freq for _, count in rle_indices for freq in [1]}, max_length=16)
    rle_huffman_sequence = _generate_rle_huffman_sequence(rle_indices, huffman_codebook, rle_huffman_codebook)
    rle_huffman_header = header_rle_huffman_function(rle_huffman_codebook)

    ### Alignment
    def finalize(header: bitarray, sequence: bitarray) -> bitarray:
        if align_to_byte_header:
            align_to_byte(header)
        if align_to_byte_sequence:
            align_to_byte(sequence)
        combined = header + sequence
        if align_to_byte_all:
            align_to_byte(combined)
        return combined

    huffman_all = finalize(huffman_header, huffman_sequence)
    rle_all = finalize(rle_header, rle_sequence)
    rle_huffman_all = finalize(rle_huffman_header, rle_huffman_sequence)

    ### Compare Total Sizes and Select Best Encoding
    results = {
        "Huffman": {"size": len(huffman_all), "sequence": huffman_sequence, "header": huffman_header, "all": huffman_all},
        "RLE": {"size": len(rle_all), "sequence": rle_sequence, "header": rle_header, "all": rle_all, "rle_bit_length": rle_bit_length},
        "RLE+Huffman": {"size": len(rle_huffman_all), "sequence": rle_huffman_sequence, "header": rle_huffman_header, "all": rle_huffman_all, "rle_huffman_codebook": rle_huffman_codebook},
    }
    best_method = min(results, key=lambda x: results[x]["size"])
    best_info = results[best_method]

    return (
        {
            "sequence": bytes(best_info["sequence"]),
            "header": bytes(best_info["header"]),
            "all": bytes(best_info["all"]),
        },
        {
            "method": best_method,
            "is_rle": best_method != "Huffman",
            "uses_huffman": best_method == "RLE+Huffman",
            **({k: v for k, v in best_info.items() if k not in ["size", "sequence", "header", "all"]}),
        }
    )
