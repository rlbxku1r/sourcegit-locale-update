# sourcegit-update-locale

## Usage
```sh
cd sourcegit/src/Resources/Locales

# Merge missing strings
sourcegit-update-locale merge ja_JP.axaml > ja_JP.merged.axaml

# Merge missing strings in 'reviewer mode' that puts the original strings alongside
sourcegit-update-locale review ja_JP.axaml > ja_JP.review.axaml
```
