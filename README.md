# sourcegit-locale-update

## Usage
```sh
cd sourcegit/src/Resources/Locales

# Merge missing strings
# (NOTE: You should tweak its final output manually, such as indentations and MergedDictionaries metadata)
sourcegit-locale-update merge ja_JP.axaml > ja_JP.merged.axaml

# Merge missing strings in 'reviewer mode' that puts the original strings alongside
sourcegit-locale-update review ja_JP.axaml > ja_JP.review.axaml
```
