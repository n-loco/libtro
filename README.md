# LibTRO
*Uma biblioteca com funções semelhantes às do C para fins educacionais na Eletrônica, IF Sul Pelotas.*

---

> [!warning]  
> Esta biblioteca ainda está em desenvolvimento.
> A API ainda esta quebrada e/ou em constante mudança.

## Como usar?
Para aprender a usar a **LibTRO** basta acessar
a [documentação de referência online](https://n-loco.github.io/libtro).

## Requisitos
 - [**Just**](https://just.systems/) >= 1.48.x;
 - **Um compilador C11** — *preferencialmente Clang, GCC ou MSVC*;
 - [**Python**](https://www.python.org/downloads/) 3;
 - [**Meson**](https://mesonbuild.com/) >= 1.10.x;
   - [**Ninja**](https://ninja-build.org/) >= 1.23.x — _**Meson** precisa_.
 - **Clang Format** >= 18.1.x — *opcional*
   > [!note]  
   > Opcional para *build*, mas **extremamente**
   > recomendado para *desenvolvimento*.
 - [**Doxygen**](https://www.doxygen.nl/) >= 1.17.x — *opcional*.

## Buildando
Bem simples.
```sh
just build
```
Os arquivos de build vão estar *normalmente* em `build/`,
mas teste caminho pode ser costumizado alterando a
*variável de ambiente* `MESON_BUILDDIR`, que inclusive
pode ser colocada em um arquivo `.env`.

## Testando, Formatando, Documentando, etc.
O arquivo [`Justfile`](./Justfile) contém
não só a tarefa `build`, mas sim todas
as tarefas importantes, para obter
ajuda simplesmente use:
```sh
just
# Ou
just help
# E também
just help tarefa
```

## Integração com IDEs e Editores de Código
### JetBrains CLion
Para o *CLion* basta ficar atento nas [dependências de projeto](./.idea/externalDependencies.xml)
e o resto a própria IDE é capaz de cuidar.

### Visual Studio Code
Para o *Visual Studio Code* fique atento nas [recomendações de extensões](./.vscode/extensions.json).

Para depurar os testes usando a extensão [CodeLLDB](https://marketplace.visualstudio.com/items?itemName=vadimcn.vscode-lldb)
(*listada nas [recomendações de extensões](./.vscode/extensions.json)*)
é muito importante executar:
```sh
just config CodeLLDB
```
que ira gerar um `launch.json` do *Visual Studio Code*
com todos os *targets* gerados pelo **Meson**.

> [!note]  
> Sempre que um *target* novo é adicionado,
> é recomendado executar esta tarefa novamente.
