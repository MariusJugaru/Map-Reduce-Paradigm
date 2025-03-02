# Map-Reduce Document Processing

## Overview
This project implements a **Map-Reduce** model similar to the one used by **Google** for processing large sets of documents in distributed systems.
The goal is to efficiently parallelize and distribute tasks across multiple processors.
Map-Reduce is a parallel model for processing large-scale documents.

## How it works
- Uses **Mappers** to extract relevant data from documents.
- Uses **Reducers** to agregate and produce final results.
- **Mapping Phase:** Each **Mapper** processes a subset of the documents and outputs key-value pairs of the form `{word, document_id}`.
- **Reducing Phase:** Each **Reducer** aggregates document IDs for each word, building the final **inverted index**.
- **Output:** The final output is a dictionary where each **word** maps to a list of **documents** containing that word.
