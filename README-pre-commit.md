# Install

We can use pre-commit binary which depend against python or
prek which was recreated in rust (faster, dependency free)

## opensuse

```bash
zypper in python313-pre-commit
```

```bash
zypper in prek
```

## Usage

We need to overwrite pre-commit config file:
pre-commit install -f or prek install -f

For executing for all file:

pre-commit run --all or prek run --all-files
