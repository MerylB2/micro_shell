 # Microshell

Mini-shell capable d'exécuter des commandes avec pipes (`|`) et séparateurs (`;`), incluant la commande built-in `cd`.

## Compilation

```bash
gcc -Wall -Wextra -Werror bon_microshell.c -o microshell
```

## Tests

### Exemples du sujet

```bash
# Test avec pipe et séparateur
./microshell /bin/ls "|" /usr/bin/grep microshell ";" /bin/echo i love my microshell

# Test avec séparateurs multiples et chaînes contenant des séparateurs
./microshell /usr/bin/echo WOOT "; microshell ;" "; ;" ";" /usr/bin/echo "is hell of fun"

# Test avec fichier inexistant et pipe
./microshell /bin/ls ./file_who_doesnt_exist ";" ";" ";" ";" ";" /bin/ls "|" /usr/bin/grep microshell ";"
```

### Tests de la commande cd

```bash
# Test cd valide
./microshell cd /tmp ";" /bin/pwd

# Test cd avec erreur (mauvais nombre d'arguments)
./microshell cd

# Test cd avec erreur (répertoire inexistant)
./microshell cd /nonexistent ";" /bin/pwd
```

### Tests d'erreurs

```bash
# Test commande inexistante
./microshell /bin/nonexistent ";" /bin/echo "after error"

# Test pipes multiples
./microshell /bin/ls "|" /usr/bin/head -5 "|" /usr/bin/tail -2
```

## Format des arguments

- Chaque élément (commande, argument, pipe, séparateur) doit être un argument séparé
- `"|"` et `";"` doivent être entre guillemets pour éviter l'interprétation du shell
- Les chemins peuvent être absolus ou relatifs (pas de construction de PATH)

## Fonctionnalités

- Exécution de commandes avec pipes (`|`)
- Séparateurs de commandes (`;`)
- Commande built-in `cd` avec gestion d'erreurs
- Gestion des erreurs selon les spécifications
- Support de centaines de pipes même avec limite de descripteurs < 30
