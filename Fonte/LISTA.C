/***************************************************************************
*  $MCI Módulo de implementação: LIS  Lista duplamente encadeada
*
*  Arquivo gerado:              LISTA.c
*  Letras identificadoras:      LIS
*
*  Nome da base de software:    Arcabouço para a automação de testes de programas redigidos em C
*  Arquivo da base de software: D:\AUTOTEST\PROJETOS\LISTA.BSW
*
*  Projeto: INF 1301 / 1628 Automatização dos testes de módulos C
*  Gestor:  LES/DI/PUC-Rio
*  Autores: avs
*           gh - Gabriel Heffer
*           ys - Yuri Strack
*
*  $HA Histórico de evolução:
*     Versão  Autor    Data     Observações
*     5       ys\gh 29/jun/2019 Ajustes nas funcoes.
*     5       ys\gh 28/jun/2019 Desenvolvimento do deturpador.
*     5       ys\gh 27/jun/2019 Desenvolvimento do verificador.
*     4       avs   01/fev/2006 criar linguagem script simbólica
*     3       avs   08/dez/2004 uniformização dos exemplos
*     2       avs   07/jul/2003 unificação de todos os módulos em um só projeto
*     1       avs   16/abr/2003 início desenvolvimento
*
***************************************************************************/

#include   <stdio.h>
#include   <string.h>
#include   <memory.h>
#include   <malloc.h>
#include   <assert.h>


#ifdef _DEBUG

#include   "CESPDIN.h"
#include   "CONTA.h"
#include   "Generico.h"
#include   "Tabelas\\IdTiposEspaco.def"

#endif

#define LISTA_OWN
#include "LISTA.h"
#undef LISTA_OWN

/***********************************************************************
*
*  $TC Tipo de dados: LIS Elemento da lista
*
*
***********************************************************************/

   typedef struct tagElemLista {

         void * pValor ;
               /* Ponteiro para o valor contido no elemento */

         #ifdef _DEBUG
         char tipoElem ;
               /* Tipo do elemento do nó da lista */
         LIS_tppLista pCabeca ;
               /* Ponteiro para a cabeça da lista */
         #endif

         struct tagElemLista * pAnt ;
               /* Ponteiro para o elemento predecessor */

         struct tagElemLista * pProx ;
               /* Ponteiro para o elemento sucessor */

   } tpElemLista ;

/***********************************************************************
*
*  $TC Tipo de dados: LIS Descritor da cabe�a de lista
*
*
***********************************************************************/

   typedef struct LIS_tagLista {

         tpElemLista * pOrigemLista ;
               /* Ponteiro para a origem da lista */

         tpElemLista * pFimLista ;
               /* Ponteiro para o final da lista */

         tpElemLista * pElemCorr ;
               /* Ponteiro para o elemento corrente da lista */

         int numElem ;
               /* N�mero de elementos da lista */

         #ifdef _DEBUG
         char tipoLista ;
               /* Tipo do elemento da lista */
         #endif

         void ( * ExcluirValor ) ( void * pValor ) ;
               /* Ponteiro para a função de destruição do valor contido em um elemento */

   } LIS_tpLista ;


/*****  Dados encapsulados no m�dulo  *****/

     #ifdef _DEBUG

     static char EspacoLixo[ 256 ] =
            "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" ;
           /* Espaço de dados lixo usado ao testar */

     #endif

/***** Protótipos das funções encapuladas no módulo *****/

   static void LiberarElemento( LIS_tppLista   pLista ,
                                tpElemLista  * pElem   ) ;

   static tpElemLista * CriarElemento( LIS_tppLista pLista ,
                                       void *       pValor  ) ;

   static void LimparCabeca( LIS_tppLista pLista ) ;

   #ifdef _DEBUG

      static int LIS_VerificarCabeca( void * pCabecaParm ) ;
      static int LIS_VerificarNo( void * pNoParm ) ;
      static int VerificarNo( tpElemLista * pElem ) ;

   #endif

/*****  Código das funções exportadas pelo módulo  *****/

/***************************************************************************
*
*  Função: LIS  &Criar lista
*  ****/

   LIS_tppLista LIS_CriarLista(
             void   ( * ExcluirValor ) ( void * pDado )
             #ifdef _DEBUG
             , char tipoLista
             #endif 
			 )
   {

      LIS_tpLista * pLista = NULL ;
      pLista = ( LIS_tpLista * ) malloc( sizeof( LIS_tpLista )) ;
	  
      if ( pLista == NULL )
      {
         return NULL ;
      } /* if */

      LimparCabeca( pLista ) ;

      pLista->ExcluirValor = ExcluirValor ;
      #ifdef _DEBUG
      pLista->tipoLista = tipoLista ;
      CED_DefinirTipoEspaco( pLista , LIS_TipoEspacoCabeca) ;
      #endif

      return pLista ;

   } /* Fim fun��o: LIS  &Criar lista */

/***************************************************************************
*
*  Fun��o: LIS  &Destruir lista
*  ****/

   void LIS_DestruirLista( LIS_tppLista pLista )
   {

      #ifdef _DEBUG
         assert( pLista != NULL ) ;
      #endif

      LIS_EsvaziarLista( pLista ) ;

      free( pLista ) ;

   } /* Fim fun��o: LIS  &Destruir lista */

/***************************************************************************
*
*  Fun��o: LIS  &Esvaziar lista
*  ****/

   void LIS_EsvaziarLista( LIS_tppLista pLista )
   {

      tpElemLista * pElem ;
      tpElemLista * pProx ;

      #ifdef _DEBUG
         assert( pLista != NULL ) ;
      #endif

      pElem = pLista->pOrigemLista ;
      while ( pElem != NULL )
      {
         pProx = pElem->pProx ;
         LiberarElemento( pLista , pElem ) ;
         pElem = pProx ;
      } /* while */

      LimparCabeca( pLista ) ;

   } /* Fim fun��o: LIS  &Esvaziar lista */

/***************************************************************************
*
*  Fun��o: LIS  &Inserir elemento antes
*  ****/

   LIS_tpCondRet LIS_InserirElementoAntes( LIS_tppLista pLista ,
                                           void * pValor
                                           #ifdef _DEBUG
                                           , char tipoElem
                                           #endif 
										   )
   {

      tpElemLista * pElem ;

      #ifdef _DEBUG
         /* assert( pLista != NULL ) ; já estava aqui antes */
         if ( pLista->tipoLista != tipoElem ){
           return LIS_CondRetErroEstrutura ;
         }
      #endif

      /* Criar elemento a inerir antes */

         pElem = CriarElemento( pLista , pValor ) ;
         if ( pElem == NULL )
         {
            return LIS_CondRetFaltouMemoria ;
         } /* if */

      /* Encadear o elemento antes do elemento corrente */

         if ( pLista->pElemCorr == NULL )
         {
            pLista->pOrigemLista = pElem ;
            pLista->pFimLista = pElem ;
         } else
         {
            if ( pLista->pElemCorr->pAnt != NULL )
            {
               pElem->pAnt  = pLista->pElemCorr->pAnt ;
               pLista->pElemCorr->pAnt->pProx = pElem ;
            } else
            {
               pLista->pOrigemLista = pElem ;
            } /* if */

            pElem->pProx = pLista->pElemCorr ;
            pLista->pElemCorr->pAnt = pElem ;
         } /* if */

         pLista->pElemCorr = pElem ;

         #ifdef _DEBUG
         pElem->tipoElem = tipoElem ;
         pElem->pCabeca = pLista ;
         CED_DefinirTipoEspaco( pElem , LIS_TipoEspacoNo ) ;
         #endif

         return LIS_CondRetOK ;

   } /* Fim fun��o: LIS  &Inserir elemento antes */

/***************************************************************************
*
*  Fun��o: LIS  &Inserir elemento ap�s
*  ****/

   LIS_tpCondRet LIS_InserirElementoApos( LIS_tppLista pLista ,
                                          void * pValor
                                          #ifdef _DEBUG
                                          , char tipoElem
                                          #endif 
										  )

   {

      tpElemLista * pElem ;

      #ifdef _DEBUG
         /* assert( pLista != NULL ) ;  já estava aqui antes */
         if ( pLista->tipoLista != tipoElem ){
           return LIS_CondRetErroEstrutura ;
         }
      #endif

      /* Criar elemento a inerir ap�s */

         pElem = CriarElemento( pLista , pValor ) ;
         if ( pElem == NULL )
         {
            return LIS_CondRetFaltouMemoria ;
         } /* if */

      /* Encadear o elemento ap�s o elemento */

         if ( pLista->pElemCorr == NULL )
         {
            pLista->pOrigemLista = pElem ;
            pLista->pFimLista = pElem ;
         } else
         {
            if ( pLista->pElemCorr->pProx != NULL )
            {
               pElem->pProx  = pLista->pElemCorr->pProx ;
               pLista->pElemCorr->pProx->pAnt = pElem ;
            } else
            {
               pLista->pFimLista = pElem ;
            } /* if */

            pElem->pAnt = pLista->pElemCorr ;
            pLista->pElemCorr->pProx = pElem ;

         } /* if */

         pLista->pElemCorr = pElem ;

         #ifdef _DEBUG
         pElem->tipoElem = tipoElem ;
         pElem->pCabeca  = pLista ;
         CED_DefinirTipoEspaco( pElem , LIS_TipoEspacoNo ) ;
         #endif
         return LIS_CondRetOK ;

   } /* Fim fun��o: LIS  &Inserir elemento ap�s */

/***************************************************************************
*
*  Fun��o: LIS  &Excluir elemento
*  ****/

   LIS_tpCondRet LIS_ExcluirElemento( LIS_tppLista pLista )
   {

      tpElemLista * pElem ;

      #ifdef _DEBUG
         assert( pLista  != NULL ) ;
      #endif

      if ( pLista->pElemCorr == NULL )
      {
         return LIS_CondRetListaVazia ;
      } /* if */

      pElem = pLista->pElemCorr ;

      /* Desencadeia � esquerda */

         if ( pElem->pAnt != NULL )
         {
            pElem->pAnt->pProx   = pElem->pProx ;
            pLista->pElemCorr    = pElem->pAnt ;
         } else {
            pLista->pElemCorr    = pElem->pProx ;
            pLista->pOrigemLista = pLista->pElemCorr ;
         } /* if */

      /* Desencadeia � direita */

         if ( pElem->pProx != NULL )
         {
            pElem->pProx->pAnt = pElem->pAnt ;
         } else
         {
            pLista->pFimLista = pElem->pAnt ;
         } /* if */

      LiberarElemento( pLista , pElem ) ;

      return LIS_CondRetOK ;

   } /* Fim fun��o: LIS  &Excluir elemento */

/***************************************************************************
*
*  Fun��o: LIS  &Obter refer�ncia para o valor contido no elemento
*  ****/

   void * LIS_ObterValor( LIS_tppLista pLista )
   {

      #ifdef _DEBUG
         assert( pLista != NULL ) ;
      #endif

      if ( pLista->pElemCorr == NULL )
      {
        return NULL ;
      } /* if */

      return pLista->pElemCorr->pValor ;

   } /* Fim fun��o: LIS  &Obter refer�ncia para o valor contido no elemento */

/***************************************************************************
*
*  Fun��o: LIS  &Ir para o elemento inicial
*  ****/

   void IrInicioLista( LIS_tppLista pLista )
   {

      #ifdef _DEBUG
         assert( pLista != NULL ) ;
      #endif

      pLista->pElemCorr = pLista->pOrigemLista ;

   } /* Fim fun��o: LIS  &Ir para o elemento inicial */

/***************************************************************************
*
*  Fun��o: LIS  &Ir para o elemento final
*  ****/

   void IrFinalLista( LIS_tppLista pLista )
   {

      #ifdef _DEBUG
         assert( pLista != NULL ) ;
      #endif

      pLista->pElemCorr = pLista->pFimLista ;

   } /* Fim fun��o: LIS  &Ir para o elemento final */

/***************************************************************************
*
*  Fun��o: LIS  &Avan�ar elemento
*  ****/

   LIS_tpCondRet LIS_AvancarElementoCorrente( LIS_tppLista pLista ,
                                              int numElem )
   {

      int i ;

      tpElemLista * pElem ;

      #ifdef _DEBUG
         assert( pLista != NULL ) ;
      #endif

      /* Tratar lista vazia */

         if ( pLista->pElemCorr == NULL )
         {

            return LIS_CondRetListaVazia ;

         } /* fim ativa: Tratar lista vazia */

      /* Tratar avan�ar para frente */

         if ( numElem > 0 )
         {

            pElem = pLista->pElemCorr ;
            for( i = numElem ; i > 0 ; i-- )
            {
               if ( pElem == NULL )
               {
                  break ;
               } /* if */
               pElem    = pElem->pProx ;
            } /* for */

            if ( pElem != NULL )
            {
               pLista->pElemCorr = pElem ;
               return LIS_CondRetOK ;
            } /* if */

            pLista->pElemCorr = pLista->pFimLista ;
            return LIS_CondRetFimLista ;

         } /* fim ativa: Tratar avan�ar para frente */

      /* Tratar avan�ar para tr�s */

         else if ( numElem < 0 )
         {

            pElem = pLista->pElemCorr ;
            for( i = numElem ; i < 0 ; i++ )
            {
               if ( pElem == NULL )
               {
                  break ;
               } /* if */
               pElem    = pElem->pAnt ;
            } /* for */

            if ( pElem != NULL )
            {
               pLista->pElemCorr = pElem ;
               return LIS_CondRetOK ;
            } /* if */

            pLista->pElemCorr = pLista->pOrigemLista ;
            return LIS_CondRetFimLista ;

         } /* fim ativa: Tratar avan�ar para tr�s */

      /* Tratar n�o avan�ar */

         return LIS_CondRetOK ;

   } /* Fim fun��o: LIS  &Avan�ar elemento */

/***************************************************************************
*
*  Fun��o: LIS  &Procurar elemento contendo valor
*  ****/

   LIS_tpCondRet LIS_ProcurarValor( LIS_tppLista pLista ,
                                    void * pValor        )
   {

      tpElemLista * pElem ;

      #ifdef _DEBUG
         assert( pLista  != NULL ) ;
      #endif

      if ( pLista->pElemCorr == NULL )
      {
         return LIS_CondRetListaVazia ;
      } /* if */

      for ( pElem  = pLista->pElemCorr ;
            pElem != NULL ;
            pElem  = pElem->pProx )
      {
         if ( pElem->pValor == pValor )
         {
            pLista->pElemCorr = pElem ;
            return LIS_CondRetOK ;
         } /* if */
      } /* for */

      return LIS_CondRetNaoAchou ;

   } /* Fim fun��o: LIS  &Procurar elemento contendo valor */


#ifdef _DEBUG

/***************************************************************************
*
*  Função: LIS  &Verificar uma Lista
*  ****/

  int LIS_VerificarLista( void * pListaParm )
  {
     int erros = 0, erroCabeca = 0, erroElem = 0;
     LIS_tpLista * pLista = NULL ;
     erroCabeca = LIS_VerificarCabeca( pListaParm );

     if ( erroCabeca != 0 )
     {
        return erroCabeca ;
     } /* if */

     CED_MarcarEspacoAtivo( pListaParm ) ;

     pLista = ( LIS_tpLista * ) ( pListaParm ) ;
	 erros += VerificarNo( pLista->pOrigemLista , pLista ) ;
	 if( erros == 0 )
	 {

		 CNT_CONTAR("SUCESSO");
	 }

     return erros;

  } /* Fim função: LIS  &Verificar uma Lista */

#endif

#ifdef _DEBUG

/***************************************************************************
*
*  Função: LIS  &Deturpar Lista
*  ****/

   void LIS_Deturpar( void * pListaParm ,
                      LIS_tpModosDeturpacao ModoDeturpa )
   {

      LIS_tppLista pLista = NULL ;

      if ( pListaParm == NULL )
      {
         return ;
      } /* if */

      pLista = ( LIS_tppLista )( pListaParm ) ;

      switch ( ModoDeturpa ) {

      /* Elimina corrente */

         case DeturpaEliCorr :
         {

            free(pLista->pElemCorr) ;

            break ;

         } /* fim ativa */

      /* Anula ponteiro proximo */

         case DeturpaProxNulo :
         {

            pLista->pElemCorr->pProx = NULL ;

            break ;

         } /* fim ativa */

      /* Anula ponteiro anterior */

         case DeturpaAnteriorNulo :
         {

            pLista->pElemCorr->pAnt = NULL ;

            break ;

         } /* fim ativa */

      /* Faz pProx apontar para lixo */

         case DeturpaProxLixo :
         {

            pLista->pElemCorr->pProx = ( tpElemLista * )( EspacoLixo ) ;

            break ;

         } /* fim ativa: Faz raiz apontar para lixo */

      /* Faz pAnt apontar para lixo */

         case DeturpaAnteriorLixo :
         {

            pLista->pElemCorr->pAnt = ( tpElemLista * )( EspacoLixo ) ;

            break ;

         } /* fim ativa */

      /* Atribui NULL ao conteudo do nó */

         case DeturpaConteudoNulo :
         {

            pLista->pElemCorr->pValor = NULL;

            break ;

         } /* fim ativa */

      /* Atribui NULL ao tipo do nó */

         case DeturpaTipoNo :
         {

            pLista->pElemCorr->tipoElem = NULL;

            break ;

         } /* fim ativa */

      /* Desencadeia no corrente */

         case DeturpaDesencadeiaNo :
         {

            if ( pLista->pElemCorr->pProx != NULL ){
              pLista->pElemCorr->pProx->pAnt = pLista->pElemCorr->pAnt ; /* bota o prox pra apontar pro anterior */
            } /* if */
            else{
              pLista->pFimLista = pLista->pElemCorr->pAnt ; /* bota o fim da lista pra ser o anterior */
            } /* else */

            if ( pLista->pElemCorr->pAnt != NULL ){
              pLista->pElemCorr->pAnt->pProx = pLista->pElemCorr->pProx ; /* bota o anterior pra apontar pro proximo */
            } /* if */
            else{
              pLista->pOrigemLista = pLista->pElemCorr->pProx ; /* bota a origem da lista pra ser o prox */
            } /* else */

            break ;

         } /* fim ativa */

      /* Atribui NULL ao elemento corrente */

         case DeturpaCorrNulo :
         {

            pLista->pElemCorr = NULL;

            break ;

         } /* fim ativa */

      /* Atribui NULL ao elemento origem */

         case DeturpaOrigemNulo :
         {

            pLista->pOrigemLista = NULL;

            break ;

         } /* fim ativa */

		 case DeturpaTipoElemNo :
		 {
			 CED_DefinirTipoEspaco( pLista->pElemCorr , CED_ID_TIPO_VALOR_NULO ) ;
			 break ;
		 }

		 case DeturpaFinalNulo :
		 {
			 pLista->pFimLista = NULL ;
			 break ;
		 }

		 case DeturpadoOrigemLixo :
		 {

			 pLista->pOrigemLista = (tpElemLista*)EspacoLixo ;
			 break ;
		 }

		 case DeturpaFimLixo :
		 {

			 pLista->pFimLista = (tpElemLista*)EspacoLixo ;
			 break ;
		 }

		 case DeturpaCorrLixo :
		 {

			 pLista->pElemCorr = (tpElemLista*)EspacoLixo ;
			 break ;
		 }

      } /* fim seleciona */

   } /* Fim função: LIS  &Deturpar Lista */

#endif

/*****  Código das funções encapsuladas no módulo  *****/


/***********************************************************************
*
*  $FC Fun��o: LIS  -Liberar elemento da lista
*
*  $ED Descri��o da fun��o
*     Elimina os espa�os apontados pelo valor do elemento e o
*     pr�prio elemento.
*
***********************************************************************/

   void LiberarElemento( LIS_tppLista   pLista ,
                         tpElemLista  * pElem   )
   {

      if ( ( pLista->ExcluirValor != NULL )
        && ( pElem->pValor != NULL        ))
      {
         pLista->ExcluirValor( pElem->pValor ) ;
      } /* if */

      #ifdef _DEBUG
      pElem->pCabeca = NULL ;
      pElem->tipoElem = 0 ;
      #endif

      free( pElem ) ;

      pLista->numElem-- ;

   } /* Fim fun��o: LIS  -Liberar elemento da lista */


/***********************************************************************
*
*  $FC Fun��o: LIS  -Criar o elemento
*
***********************************************************************/

   tpElemLista * CriarElemento( LIS_tppLista pLista ,
                                void *       pValor  )
   {

      tpElemLista * pElem ;

      pElem = ( tpElemLista * ) malloc( sizeof( tpElemLista )) ;
      if ( pElem == NULL )
      {
         return NULL ;
      } /* if */

      pElem->pValor = pValor ;
      pElem->pAnt   = NULL  ;
      pElem->pProx  = NULL  ;

      pLista->numElem ++ ;

      return pElem ;

   } /* Fim fun��o: LIS  -Criar o elemento */


/***********************************************************************
*
*  $FC Fun��o: LIS  -Limpar a cabe�a da lista
*
***********************************************************************/

   void LimparCabeca( LIS_tppLista pLista )
   {

      pLista->pOrigemLista = NULL ;
      pLista->pFimLista = NULL ;
      pLista->pElemCorr = NULL ;
      pLista->numElem   = 0 ;
      #ifdef _DEBUG
      pLista->tipoLista = 0 ;
      #endif

   } /* Fim fun��o: LIS  -Limpar a cabe�a da lista */

#ifdef _DEBUG

/***********************************************************************
*
*  $FC Função: LIS  &Verificar um nó cabeça
*
*  $ED Descrição da função
*     Função da interface de teste.
*     Verifica a integridade de um nó cabeça da Lista.
*
*  $EP Parametros
*     $P pCabecaParm - ponteiro para um espaço que deveria ser uma cabeça
*                      de Lista.
*
*  $FV Valor retornado
*     Condição de retorno de teste
*
***********************************************************************/

  int LIS_VerificarCabeca( void * pCabecaParm )
  {
     int erros = 0;
     LIS_tpLista * pLista = NULL ;

        pLista = ( LIS_tpLista * )( pCabecaParm ) ;

        /* Verifica origem da Lista */

           if ( pLista->pOrigemLista != NULL )
           {
			   if ( pLista->pOrigemLista == (tpElemLista*)EspacoLixo )
			   {
				   TST_NotificarFalha("Ponteiro para Origem eh lixo.") ;
				   CNT_CONTAR("ORIG_LIXO");
				   erros++;
			   }
              if ( TST_CompararPonteiro( pCabecaParm , pLista->pOrigemLista->pCabeca ,
                   "Elemento origem nao aponta para cabeca." ) != TST_CondRetOK )
              {
                 CNT_CONTAR("ORIG_APONTA_CAB_NULO");
                 erros++ ;
              } /* if */
           } else {
              if ( TST_CompararPonteiro( NULL , pLista->pElemCorr ,
                   "Lista vazia tem elemento corrente não NULL." ) != TST_CondRetOK )
              {
                 CNT_CONTAR("LIS_VAZIA_COM_ELEM_NAONULO");
                 erros++ ;
              } /* if */
           } /* if */

        /* Verifica fim da Lista */

           if ( pLista->pFimLista != NULL )
           {
			   if ( pLista->pFimLista == (tpElemLista*)EspacoLixo )
			   {
				   TST_NotificarFalha("Ponteiro para Fim eh lixo.") ;
				   CNT_CONTAR("FIM_LIXO");
				   erros++;
			   }
              if ( TST_CompararPonteiro( pCabecaParm , pLista->pFimLista->pCabeca ,
                   "Elemento final nao aponta para cabeca." ) != TST_CondRetOK )
              {
                 CNT_CONTAR("FIM_APONTA_CAB_NULO");
                 erros++ ;
              } /* if */
           } else {
              if ( TST_CompararPonteiro( NULL , pLista->pOrigemLista ,
                   "Lista não vazia tem elemento final NULL." ) != TST_CondRetOK )
              {
                 CNT_CONTAR("FIM_NULO_LIS_NAO_VAZIA");
                 erros++ ;
              } /* if */
           } /* if */

     /* Verifica corrente */

        if ( pLista->pElemCorr != NULL )
        {
			if ( pLista->pElemCorr == (tpElemLista*)EspacoLixo )
			   {
				   TST_NotificarFalha("Ponteiro para Corrente eh lixo.") ;
				   CNT_CONTAR("CORR_LIXO");
				   erros++;
			   }
           if ( TST_CompararPonteiro( pCabecaParm , pLista->pElemCorr->pCabeca ,
                "Elemento corrente nao aponta para cabeca." ) != TST_CondRetOK )
           {
              CNT_CONTAR("CORR_APONTA_CAB_NULO");
              erros++ ;
           } /* if */
        } else {
           if ( TST_CompararPonteiro( NULL , pLista->pOrigemLista ,
                "Lista nao vazia tem elemento corrente NULL." ) != TST_CondRetOK )
           {
              CNT_CONTAR("CORR_NULO_LIS_NAO_VAZIA");
              erros++ ;
           } /* if */
        } /* if */

     return erros ;

  } /* Fim função: LIS  &Verificar um nó cabeça */

#endif

#ifdef _DEBUG

/***********************************************************************
*
*  $FC Função: LIS  &Verificar um nó de estrutura
*
*  $ED Descrição da função
*     Função da interface de teste.
*     Verifica a integridade de um nó estrutural da Lista.
*
*  $EP Parametros
*     $P pNoParm - ponteiro para espaço que deveria ser um nó estrutural.
*
*  $FV Valor retornado
*     Número de erros encontrados.
*
***********************************************************************/

   int LIS_VerificarNo( void * pNoParm )
   {
      LIS_tppLista lista = NULL;
      tpElemLista* pCorr = NULL;
      int erros = 0;
      /* Verificar se eh elemento estrutural */
		 if (TST_CompararInt( LIS_TipoEspacoNo ,
             CED_ObterTipoEspaco( pNoParm ) ,
             "Tipo do espaco de dados nao eh no de Lista." ) != TST_CondRetOK)
         {
            CNT_CONTAR("ELEM_TIPO_ERRADO");
            erros ++ ;
         } /* if */

         pCorr = (tpElemLista*)pNoParm;
         lista = pCorr->pCabeca;
      /*Verificar tipo */
         if (TST_CompararChar(pCorr->tipoElem,lista->tipoLista,
             "Elemento corrente tem tipo diferente do tipo da lista.") != TST_CondRetOK )
         {
           CNT_CONTAR("TIPO_ELEM_DIF_TIPO_LISTA");
           erros++;
         }

      /* Verificar ponteiro da cabeca do elemento */

           erros += LIS_VerificarCabeca( lista );

      /* Verificar ponteiro do elemento anterior */
           if (pCorr->pAnt != NULL)
           {
             if (pCorr->pAnt->pProx != pCorr)
             {
               CNT_CONTAR("ERRO_ESTR_LIS1");
               TST_NotificarFalha( "pAnt->pProx nao aponta para o corrente." );
               erros++;
             }
			 if ( pCorr->pAnt == (tpElemLista*)EspacoLixo )
			   {
				   CNT_CONTAR("TIPO_ELEM_LIXO");
				   TST_NotificarFalha( "Ponteiro para elemento eh lixo." ) ;
				   return erros + 1 ;
			   }
           }else{
             if ( TST_CompararPonteiro(pCorr, lista->pOrigemLista,
                  "Elemento deveria ser origem, pCorr->pAnt eh nulo." ) != TST_CondRetOK )
             {
               CNT_CONTAR("ERRO_ESTR_LIS2");
               erros++;
             }
           }
      /* Verificar ponteiro do proximo elemento */
           if (pCorr->pProx != NULL)
           {
             if (pCorr->pProx->pAnt != pCorr)
             {
               CNT_CONTAR("ERRO_ESTR_LIS3");
               TST_NotificarFalha( "pProx->pAnt  nao aponta para o corrente." );
               erros++;
             }
           }else{
             if ( TST_CompararPonteiro(pCorr, lista->pFimLista,
                  "Elemento deveria ser fim, pCorr->pProx eh nulo." ) != TST_CondRetOK )
             {
               CNT_CONTAR("ERRO_ESTR_LIS4");
               erros++;
             }
           }
      /* Verificar ponteiro do conteudo do elemento */
           if (pCorr->pValor == NULL)
           {
             CNT_CONTAR("VALOR__ELEM_NULO") ;
			 TST_NotificarFalha("Ponteiro para o conteudo do elemento eh nulo.");
			 erros++;
           }else{
			 CNT_CONTAR("VALOR__ELEM_NAO_NULO");
		   }

      return erros ;

   } /* Fim função: LIS  &Verificar um elemento da estrutura */

#endif

#ifdef _DEBUG
/***********************************************************************
*
*  $FC Função: LIS  -Explorar verificando os elementos de uma Lista
*
*  $ED Descrição da função
*     Percorre a lista verificando os elementos à medida que forem
*     visitados. Caso seja encontrado alguma falha, funcao pode voar.
*     Portanto, no caso de falha, é possível que nem todos
*     os elementos da lista sejam visitados.
*
***********************************************************************/

  int VerificarNo( tpElemLista * pElem , LIS_tppLista pLista )
  {
     int erros = 0;
     int numElem = 0;
     tpElemLista* pAux = NULL;
     if ( pElem == NULL )
     {
        return 0 ;
     }
	 if ( pLista->pElemCorr->pProx == NULL && pLista->pElemCorr != pLista->pFimLista )
	 {
		 pAux = pLista->pFimLista ;
		 while( 1 )
		 {
			 if ( pAux == pLista->pElemCorr )
			 {
				CNT_CONTAR("ELEM_NULO");
				TST_NotificarFalha( "Tentou verificar elemento inexistente." ) ;
				erros++;
				break;
			 }
			 pAux = pAux->pAnt ;
		 }
	 }
     pAux = pElem;
     while (pAux != NULL){
		 if ( pAux == (tpElemLista*)EspacoLixo )
		 {
			 CNT_CONTAR("TIPO_ELEM_LIXO");
			 TST_NotificarFalha( "Ponteiro para elemento eh lixo." ) ;
			 return erros + 1 ;
		 }/* if */
       CED_MarcarEspacoAtivo( pAux ) ;
       erros += LIS_VerificarNo(pAux);
       numElem ++;
       pAux = pAux->pProx;
     }
     if (numElem != pLista->numElem){
       erros++;
	   TST_NotificarFalha("Numero de elementos percorridos eh diferente do numero de elementos da cabeca"); 
       CNT_CONTAR("NUMERO_ELEM_ERRADO");
     }
     return erros ;

  } /* Fim função: LIS  -Explorar verificando os elementos de uma Lista */

#endif
/********** Fim do m�dulo de implementa��o: LIS  Lista duplamente encadeada **********/
