import requests
import argparse
import json

def fetch(url, start, length):
    headers = {"Range": "bytes={}-{}".format(start, start+length)}
    r = requests.get(url, headers=headers)
    return r.text.rstrip().rstrip(',').rstrip(']')

def prettify_json(text):
    return json.dumps(json.loads(text), indent=2)

def main():
    parser = argparse.ArgumentParser(
                    prog = 'json-fetcher',
                    description = 'Fetches subset of the big json indexed by json-indexer based on npi',
                    epilog = 'Text at the bottom of help')
    parser.add_argument('npi')
    parser.add_argument('--db')
    parser.add_argument('--url')
    args = parser.parse_args()

    for line in open(args.db).readlines():
        (npi, start, length) = line.split(',')

        if npi == args.npi:
            print(prettify_json(fetch(args.url, int(start), int(length))))

if __name__ == '__main__':
    main()
