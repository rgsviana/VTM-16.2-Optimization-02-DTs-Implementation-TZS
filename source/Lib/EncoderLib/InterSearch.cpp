//RAMIRO - VTM 16.2 - Otimização Inter-Quadros - Implementação de DTs no TZS

/* The copyright in this software is being made available under the BSD
     * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2022, ITU/ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \file     EncSearch.cpp
 *  \brief    encoder inter search class
 */

#include "InterSearch.h"


#include "CommonLib/CommonDef.h"
#include "CommonLib/Rom.h"
#include "CommonLib/MotionInfo.h"
#include "CommonLib/Picture.h"
#include "CommonLib/UnitTools.h"
#include "CommonLib/dtrace_next.h"
#include "CommonLib/dtrace_buffer.h"
#include "CommonLib/MCTS.h"

//RAMIRO
#include "CommonLib/extracaoDados.h"
#include <chrono>

#include "EncModeCtrl.h"
#include "EncLib.h"

#include <math.h>
#include <limits>

//RAMIRO: função para calcular e extrair features
#if EXTRA_FEATURES
    
    std::unordered_map<unsigned long int, dadosBloco> mapaBlocos;
    unsigned long int idBlocoTemp=1, idBlocoAtual = 2, idBlocoPaiViz = 1;
    int POCAtual = -1, POCTemp = -2;
    int affBitsL0 = 0, affBitsL1 = 0;
    Distortion custoTemp = std::numeric_limits<Distortion>::max();
    Distortion custoTempTeste = std::numeric_limits<Distortion>::max();
    Distortion melhorCustoAtual = std::numeric_limits<Distortion>::max();
    dadosBloco db, dadosPaiViz;
    bool atualizaMapa = false, blocoAffAvaliavel = false, blocosIguais = false, buscaPaiVizinho = true;
    
    std::string strIdBlocoPaiViz;
    
    //RAMIRO: dados gerais para extração
    static int POC;
    static int depth;
    static int qtDepth;
    static int mtDepht;
    static int videoWidth;
    static int videoHeight;
    static int cuPosX;
    static int cuPosY;
    static int cuWidth;
    static int cuHeight;
    static int bcw_Index;
    static int IMV;   
    static int mvUniL0HorX;
    static int mvUniL0VerY;
    static int mvUniL1HorX;
    static int mvUniL1VerY;    
    static int mvPredUniL0HorX;
    static int mvPredUniL0VerY;
    static int mvPredUniL1HorX;
    static int mvPredUniL1VerY;    
    static uint64_t custoMvUniL0;
    static uint64_t custoMvUniL1;
    static uint64_t custoMvUniL0_1;
    static uint64_t custoMvUniL1_1;    
    static int bitsMvUniL0;
    static int bitsMvUniL1;    
    static int mvBiL0HorX;
    static int mvBiL0VerY;
    static int mvBiL1HorX;
    static int mvBiL1VerY;    
    static int mvPredBiL0HorX;
    static int mvPredBiL0VerY;
    static int mvPredBiL1HorX;
    static int mvPredBiL1VerY;    
    static uint64_t custoBi;
    static int bitsMvBi;    
    static int SMVD;
    static int interDir;
    static int atualQP;    
    
    //RAMIRO: dados da Affine para extração
    static int affinePai;
    static uint64_t custoPai;
    static int IMV_Pai;    
    static int affineVizEsq;
    static int affineVizEsq_1;
    static uint64_t custoVizEsq;
    static int IMV_VizEsq;    
    static int affineVizAci;
    static int affineVizAci_1;
    static uint64_t custoVizAci;
    static int IMV_VizAci;
    static int cuAtualAffine;
    static uint64_t affCustoMvUniL0;
    static uint64_t affCustoMvUniL1;
    static uint64_t affCustoMvUniL0_1;
    static uint64_t affCustoMvUniL1_1;    
    static uint64_t affCustoMvBi;    
    static int affMvUniL0P0HorX;
    static int affMvUniL0P0VerY;
    static int affMvUniL0P1HorX;
    static int affMvUniL0P1VerY;    
    static int affMvUniL1P0HorX;
    static int affMvUniL1P0VerY;
    static int affMvUniL1P1HorX;
    static int affMvUniL1P1VerY;    
    static int affMvPredUniL0P0HorX;
    static int affMvPredUniL0P0VerY;
    static int affMvPredUniL0P1HorX;
    static int affMvPredUniL0P1VerY;    
    static int affMvPredUniL1P0HorX;
    static int affMvPredUniL1P0VerY;
    static int affMvPredUniL1P1HorX;
    static int affMvPredUniL1P1VerY;    
    static int affMvBiL0P0HorX;
    static int affMvBiL0P0VerY;
    static int affMvBiL0P1HorX;
    static int affMvBiL0P1VerY;
    static int affMvBiL1P0HorX;
    static int affMvBiL1P0VerY;
    static int affMvBiL1P1HorX;
    static int affMvBiL1P1VerY;
    static int affMvPredBiL0P0HorX;
    static int affMvPredBiL0P0VerY;
    static int affMvPredBiL0P1HorX;
    static int affMvPredBiL0P1VerY;
    static int affMvPredBiL1P0HorX;
    static int affMvPredBiL1P0VerY;
    static int affMvPredBiL1P1HorX;
    static int affMvPredBiL1P1VerY; 
    static int affbitsMvUniL0;
    static int affBitsMvUniL1;
    static int affBitsMvBi;
    static int affInterDir;
    static int affTypePai;
    static int affTypeVizEsq;
    static int affTypeVizAci;
    static int cuAtualAffineType;
    
    //RAMIRO: dados do TZS para extração
    static int rui_SAD;
    static int cStruct_iBestX;
    static int cStruct_iBestY;
    //static int executaTZS;

    double features_TZS[25];

#endif

//RAMIRO: variaveis definidas para contagens
long int n_TZSearch = 0;
long int n_TZSearchHelp = 0;
long int n_TZ2PointSearch = 0;
long int n_TZ8PointDiamondSearch = 0;
long int n_TZSearchSelective = 0;
long int n_MVPrediction = 0;
long int n_FirstSearch = 0;
long int n_RasterSearch1 = 0;
long int n_RasterSearch2 = 0;
long int n_RasterRefinement = 0;
long int n_StarRefinement = 0;

//RAMIRO: variaveis definidas para temporizadores
long int totalClockTZSearch = 0;
long int totalClockTZSearchHelp = 0;
long int totalClockTZ2PointSearch = 0;
long int totalClockTZ8PointDiamondSearch = 0;
long int totalClockTZSearchSelective = 0;
long int totalClockMVPrediction = 0;
long int totalClockFirstSearch = 0;
long int totalClockRasterSearch1 = 0;
long int totalClockRasterSearch2 = 0;
long int totalClockRasterRefinement = 0;
long int totalClockStarRefinement = 0;

//RAMIRO: variaveis definidas para avaliação da Affine
long int n_Affine = 0;
long int n_Affine_4par = 0;
long int n_Affine_6par = 0;
long int totalClockAffine = 0;
long int totalClockAffine_4par = 0;
long int totalClockAffine_6par = 0;

//RAMIRO: variáveis para transpor o tamanho das CUs
long int CUW;
long int CUH;

//RAMIRO: variáveis para testar DTs
long int TZS0 = 0;
long int TZS1 = 0;
long int totalClock_DTs_TZS = 0;

/*
//RAMIRO: definidores dos puladores (0 mantem, 1 pula)
long int pula_TZSearch = 0;
long int pula_TZSearchHelp = 0;
long int pula_TZ2PointSearch = 0;
long int pula_TZ8PointDiamondSearch = 0;
long int pula_TZSearchSelective = 0;
long int pula_MVPrediction = 0;
long int pula_FirstSearch = 0;
long int pula_RasterSearch = 0;
long int pula_RefinementSearch = 0;
*/

//RAMIRO
//int testaTZS = 0;

 //! \ingroup EncoderLib
 //! \{

static const Mv s_acMvRefineH[9] =
{
  Mv(  0,  0 ), // 0
  Mv(  0, -1 ), // 1
  Mv(  0,  1 ), // 2
  Mv( -1,  0 ), // 3
  Mv(  1,  0 ), // 4
  Mv( -1, -1 ), // 5
  Mv(  1, -1 ), // 6
  Mv( -1,  1 ), // 7
  Mv(  1,  1 )  // 8
};

static const Mv s_acMvRefineQ[9] =
{
  Mv(  0,  0 ), // 0
  Mv(  0, -1 ), // 1
  Mv(  0,  1 ), // 2
  Mv( -1, -1 ), // 5
  Mv(  1, -1 ), // 6
  Mv( -1,  0 ), // 3
  Mv(  1,  0 ), // 4
  Mv( -1,  1 ), // 7
  Mv(  1,  1 )  // 8
};


InterSearch::InterSearch()
  : m_modeCtrl                    (nullptr)
  , m_pSplitCS                    (nullptr)
  , m_pFullCS                     (nullptr)
  , m_pcEncCfg                    (nullptr)
  , m_pcTrQuant                   (nullptr)
  , m_pcReshape                   (nullptr)
  , m_iSearchRange                (0)
  , m_bipredSearchRange           (0)
  , m_motionEstimationSearchMethod(MESEARCH_FULL)
  , m_CABACEstimator              (nullptr)
  , m_CtxCache                    (nullptr)
  , m_pTempPel                    (nullptr)
  , m_isInitialized               (false)
{
  for (int i=0; i<MAX_NUM_REF_LIST_ADAPT_SR; i++)
  {
    memset (m_aaiAdaptSR[i], 0, MAX_IDX_ADAPT_SR * sizeof (int));
  }
  for (int i=0; i<AMVP_MAX_NUM_CANDS+1; i++)
  {
    memset (m_auiMVPIdxCost[i], 0, (AMVP_MAX_NUM_CANDS+1) * sizeof (uint32_t) );
  }

  setWpScalingDistParam( -1, REF_PIC_LIST_X, nullptr );
  m_affMVList = nullptr;
#if GDR_ENABLED
  m_affMVListSolid = nullptr;
#endif
  m_affMVListSize = 0;
  m_affMVListIdx = 0;
  m_uniMvList = nullptr;
  m_uniMvListSize = 0;
  m_uniMvListIdx = 0;
  m_histBestSbt    = MAX_UCHAR;
  m_histBestMtsIdx = MAX_UCHAR;

}


void InterSearch::destroy()
{
  CHECK(!m_isInitialized, "Not initialized");
  if ( m_pTempPel )
  {
    delete [] m_pTempPel;
    m_pTempPel = nullptr;
  }

  m_pSplitCS = m_pFullCS = nullptr;

  m_pSaveCS = nullptr;

  for(uint32_t i = 0; i < NUM_REF_PIC_LIST_01; i++)
  {
    m_tmpPredStorage[i].destroy();
  }
  m_tmpStorageLCU.destroy();
  m_tmpAffiStorage.destroy();

  if (m_tmpAffiError != nullptr)
  {
    delete[] m_tmpAffiError;
  }
  if (m_tmpAffiDeri[0] != nullptr)
  {
    delete[] m_tmpAffiDeri[0];
  }
  if (m_tmpAffiDeri[1] != nullptr)
  {
    delete[] m_tmpAffiDeri[1];
  }
  if (m_affMVList)
  {
    delete[] m_affMVList;
    m_affMVList = nullptr;
  }
#if GDR_ENABLED
  if (m_affMVListSolid)
  {
    delete[] m_affMVListSolid;
    m_affMVListSolid = nullptr;
  }
#endif

  m_affMVListIdx = 0;
  m_affMVListSize = 0;
  if (m_uniMvList)
  {
    delete[] m_uniMvList;
    m_uniMvList = nullptr;
  }
  m_uniMvListIdx = 0;
  m_uniMvListSize = 0;
  m_isInitialized = false;

  //RAMIRO: printando
  printf("\nTESTES TEST ZONE SEARCH:\n");
  printf("Repeticoes do TZSearch: %ld\n", n_TZSearch);
  printf("Tempo do TZSearch: %12.3f\n", totalClockTZSearch * 1.0 / CLOCKS_PER_SEC);  
  /*
  printf("\n");
  printf("Repeticoes do TZSearchHelp: %ld\n", n_TZSearchHelp);
  printf("Tempo do TZSearchHelp: %12.3f\n", totalClockTZSearchHelp * 1.0 / CLOCKS_PER_SEC);  
  printf("Repeticoes do TZ2PointSearch: %ld\n", n_TZ2PointSearch);
  printf("Tempo do TZ2PointSearch: %12.3f\n", totalClockTZ2PointSearch * 1.0 / CLOCKS_PER_SEC);  
  printf("Repeticoes do TZ8PointDiamondSearch: %ld\n", n_TZ8PointDiamondSearch);
  printf("Tempo do TZ8PointDiamondSearch: %12.3f\n", totalClockTZ8PointDiamondSearch * 1.0 / CLOCKS_PER_SEC);  
  printf("Repeticoes do TZSearchSelective: %ld\n", n_TZSearchSelective);
  printf("Tempo do TZSearchSelective: %12.3f\n", totalClockTZSearchSelective * 1.0 / CLOCKS_PER_SEC);  
  printf("\n");
  */
  printf("Repeticoes do MVPrediction: %ld\n", n_MVPrediction);
  printf("Tempo do MVPrediction: %12.3f\n", totalClockMVPrediction * 1.0 / CLOCKS_PER_SEC);  
  printf("Repeticoes do FirstSearch: %ld\n", n_FirstSearch);
  printf("Tempo do FirstSearch: %12.3f\n", totalClockFirstSearch * 1.0 / CLOCKS_PER_SEC);  
  printf("Repeticoes do RasterSearch1 (if distance is too big): %ld\n", n_RasterSearch1);
  printf("Tempo do RasterSearch1 (if distance is too big): %12.3f\n", totalClockRasterSearch1 * 1.0 / CLOCKS_PER_SEC);  
  printf("Repeticoes do RasterSearch2 (if distance is not too big): %ld\n", n_RasterSearch2);
  printf("Tempo do RasterSearch2 (if distance is not too big): %12.3f\n", totalClockRasterSearch2 * 1.0 / CLOCKS_PER_SEC);  
  printf("Repeticoes do RasterRefinement: %ld\n", n_RasterRefinement);
  printf("Tempo do RasterRefinement: %12.3f\n", totalClockRasterRefinement * 1.0 / CLOCKS_PER_SEC);  
  printf("Repeticoes do StarRefinement: %ld\n", n_StarRefinement);
  printf("Tempo do StarRefinement: %12.3f\n", totalClockStarRefinement * 1.0 / CLOCKS_PER_SEC);  
  printf("\nTESTES AFFINE:\n");
  printf("Repeticoes da Affine: %ld\n", n_Affine);
  printf("Tempo da Affine: %12.3f\n", totalClockAffine * 1.0 / CLOCKS_PER_SEC);  
  printf("Repeticoes da Affine 4-parametros: %ld\n", n_Affine_4par);
  printf("Tempo da Affine 4-parametros: %12.3f\n", totalClockAffine_4par * 1.0 / CLOCKS_PER_SEC);  
  printf("Repeticoes da Affine 6-parametros: %ld\n", n_Affine_6par);
  printf("Tempo da Affine 6-parametros: %12.3f\n", totalClockAffine_6par * 1.0 / CLOCKS_PER_SEC);  
  printf("\nANALISE DTS:\n");
  printf("Vezes que o TZS foi pulado: %ld\n", TZS0);
  printf("Vezes que o TZS foi mantido: %ld\n", TZS1);
  printf("Tempo das DTs no TZS: %12.3f\n", totalClock_DTs_TZS * 1.0 / CLOCKS_PER_SEC);  
}

void InterSearch::setTempBuffers( CodingStructure ****pSplitCS, CodingStructure ****pFullCS, CodingStructure **pSaveCS )
{
  m_pSplitCS = pSplitCS;
  m_pFullCS  = pFullCS;
  m_pSaveCS  = pSaveCS;
}

InterSearch::~InterSearch()
{
  if (m_isInitialized)
  {
    destroy();
  }
}

void InterSearch::init( EncCfg*        pcEncCfg,
                        TrQuant*       pcTrQuant,
                        int            iSearchRange,
                        int            bipredSearchRange,
                        MESearchMethod motionEstimationSearchMethod,
                        bool           useCompositeRef,
                        const uint32_t     maxCUWidth,
                        const uint32_t     maxCUHeight,
                        const uint32_t     maxTotalCUDepth,
                        RdCost*        pcRdCost,
                        CABACWriter*   CABACEstimator,
                        CtxCache*      ctxCache
                      , EncReshape*    pcReshape
)
{
  CHECK(m_isInitialized, "Already initialized");
  m_numBVs = 0;
  for (int i = 0; i < IBC_NUM_CANDIDATES; i++)
  {
    m_defaultCachedBvs.m_bvCands[i].setZero();
  }
  m_defaultCachedBvs.currCnt = 0;
  m_pcEncCfg                     = pcEncCfg;
  m_pcTrQuant                    = pcTrQuant;
  m_iSearchRange                 = iSearchRange;
  m_bipredSearchRange            = bipredSearchRange;
  m_motionEstimationSearchMethod = motionEstimationSearchMethod;
  m_CABACEstimator               = CABACEstimator;
  m_CtxCache                     = ctxCache;
  m_useCompositeRef              = useCompositeRef;
  m_pcReshape                    = pcReshape;

  for( uint32_t iDir = 0; iDir < MAX_NUM_REF_LIST_ADAPT_SR; iDir++ )
  {
    for (uint32_t refIdx = 0; refIdx < MAX_IDX_ADAPT_SR; refIdx++)
    {
      m_aaiAdaptSR[iDir][refIdx] = iSearchRange;
    }
  }

  // initialize motion cost
  for( int iNum = 0; iNum < AMVP_MAX_NUM_CANDS + 1; iNum++ )
  {
    for (int idx = 0; idx < AMVP_MAX_NUM_CANDS; idx++)
    {
      if (idx < iNum)
      {
        m_auiMVPIdxCost[idx][iNum] = xGetMvpIdxBits(idx, iNum);
      }
      else
      {
        m_auiMVPIdxCost[idx][iNum] = MAX_UINT;
      }
    }
  }

  const ChromaFormat cform = pcEncCfg->getChromaFormatIdc();
  InterPrediction::init( pcRdCost, cform, maxCUHeight );

  for( uint32_t i = 0; i < NUM_REF_PIC_LIST_01; i++ )
  {
    m_tmpPredStorage[i].create( UnitArea( cform, Area( 0, 0, MAX_CU_SIZE, MAX_CU_SIZE ) ) );
  }
  m_tmpStorageLCU.create( UnitArea( cform, Area( 0, 0, MAX_CU_SIZE, MAX_CU_SIZE ) ) );
  m_tmpAffiStorage.create( UnitArea( cform, Area( 0, 0, MAX_CU_SIZE, MAX_CU_SIZE ) ) );
  m_tmpAffiError = new Pel[MAX_CU_SIZE * MAX_CU_SIZE];
  m_tmpAffiDeri[0] = new int[MAX_CU_SIZE * MAX_CU_SIZE];
  m_tmpAffiDeri[1] = new int[MAX_CU_SIZE * MAX_CU_SIZE];
  m_pTempPel = new Pel[maxCUWidth*maxCUHeight];
  m_affMVListMaxSize = pcEncCfg->getIsLowDelay() ? AFFINE_ME_LIST_SIZE_LD : AFFINE_ME_LIST_SIZE;
  if (!m_affMVList)
  {
    m_affMVList = new AffineMVInfo[m_affMVListMaxSize];
#if GDR_ENABLED
    if (!m_affMVListSolid)
    {
      m_affMVListSolid = new AffineMVInfoSolid[m_affMVListMaxSize];
    }
#endif
  }
  m_affMVListIdx = 0;
  m_affMVListSize = 0;
  m_uniMvListMaxSize = 15;
  if (!m_uniMvList)
  {
    m_uniMvList = new BlkUniMvInfo[m_uniMvListMaxSize];
  }
  m_uniMvListIdx = 0;
  m_uniMvListSize = 0;
  m_isInitialized = true;
}

void InterSearch::resetSavedAffineMotion()
{
  for ( int i = 0; i < 2; i++ )
  {
    for ( int j = 0; j < 2; j++ )
    {
      m_affineMotion.acMvAffine4Para[i][j] = Mv( 0, 0 );
      m_affineMotion.acMvAffine6Para[i][j] = Mv( 0, 0 );
#if GDR_ENABLED
      m_affineMotion.acMvAffine4ParaSolid[i][j] = true;
      m_affineMotion.acMvAffine6ParaSolid[i][j] = true;
#endif
    }
    m_affineMotion.acMvAffine6Para[i][2] = Mv( 0, 0 );
#if GDR_ENABLED
    m_affineMotion.acMvAffine6ParaSolid[i][2] = true;
#endif

    m_affineMotion.affine4ParaRefIdx[i] = -1;
    m_affineMotion.affine6ParaRefIdx[i] = -1;
  }
  for ( int i = 0; i < 3; i++ )
  {
    m_affineMotion.hevcCost[i] = std::numeric_limits<Distortion>::max();
  }
  m_affineMotion.affine4ParaAvail = false;
  m_affineMotion.affine6ParaAvail = false;
}

#if GDR_ENABLED
void InterSearch::storeAffineMotion(Mv acAffineMv[2][3], bool acAffineMvSolid[2][3], int16_t affineRefIdx[2], EAffineModel affineType, int bcwIdx)
#else
void InterSearch::storeAffineMotion( Mv acAffineMv[2][3], int16_t affineRefIdx[2], EAffineModel affineType, int bcwIdx )
#endif
{
  if ( ( bcwIdx == BCW_DEFAULT || !m_affineMotion.affine6ParaAvail ) && affineType == AFFINEMODEL_6PARAM )
  {
    for ( int i = 0; i < 2; i++ )
    {
      for ( int j = 0; j < 3; j++ )
      {
        m_affineMotion.acMvAffine6Para[i][j] = acAffineMv[i][j];
#if GDR_ENABLED
        m_affineMotion.acMvAffine6ParaSolid[i][j] = acAffineMvSolid[i][j];
#endif
      }
      m_affineMotion.affine6ParaRefIdx[i] = affineRefIdx[i];
    }
    m_affineMotion.affine6ParaAvail = true;
  }

  if ( ( bcwIdx == BCW_DEFAULT || !m_affineMotion.affine4ParaAvail ) && affineType == AFFINEMODEL_4PARAM )
  {
    for ( int i = 0; i < 2; i++ )
    {
      for ( int j = 0; j < 2; j++ )
      {
        m_affineMotion.acMvAffine4Para[i][j] = acAffineMv[i][j];
#if GDR_ENABLED
        m_affineMotion.acMvAffine4ParaSolid[i][j] = acAffineMvSolid[i][j];
#endif
      }
      m_affineMotion.affine4ParaRefIdx[i] = affineRefIdx[i];
    }
    m_affineMotion.affine4ParaAvail = true;
  }
}

//RAMIRO: Função xTZSearchHelp
inline void InterSearch::xTZSearchHelp( IntTZSearchStruct& rcStruct, const int iSearchX, const int iSearchY, const uint8_t ucPointNr, const uint32_t uiDistance )
{
    //RAMIRO: contar as repetições do TZSearchHelp
    n_TZSearchHelp++;

    //RAMIRO: inicio do timer do TZSearchHelp
    clock_t iniClockTZSearchHelp, fimClockTZSearchHelp;
    iniClockTZSearchHelp = clock();
    
    Distortion  uiSad = 0;

//  CHECK(!( !( rcStruct.searchRange.left > iSearchX || rcStruct.searchRange.right < iSearchX || rcStruct.searchRange.top > iSearchY || rcStruct.searchRange.bottom < iSearchY )), "Unspecified error");

  const Pel* const  piRefSrch = rcStruct.piRefY + iSearchY * rcStruct.iRefStride + iSearchX;

  m_cDistParam.cur.buf = piRefSrch;

  if( 1 == rcStruct.subShiftMode )
  {
    // motion cost
    Distortion uiBitCost = m_pcRdCost->getCostOfVectorWithPredictor( iSearchX, iSearchY, rcStruct.imvShift );

    // Skip search if bit cost is already larger than best SAD
    if (uiBitCost < rcStruct.uiBestSad)
    {
      Distortion uiTempSad = m_cDistParam.distFunc( m_cDistParam );

      if((uiTempSad + uiBitCost) < rcStruct.uiBestSad)
      {
        // it's not supposed that any member of DistParams is manipulated beside cur.buf
        int subShift = m_cDistParam.subShift;
        const Pel* pOrgCpy = m_cDistParam.org.buf;
        uiSad += uiTempSad >> m_cDistParam.subShift;

        while( m_cDistParam.subShift > 0 )
        {
          int isubShift           = m_cDistParam.subShift -1;
          m_cDistParam.org.buf = rcStruct.pcPatternKey->buf + (rcStruct.pcPatternKey->stride << isubShift);
          m_cDistParam.cur.buf = piRefSrch + (rcStruct.iRefStride << isubShift);
          uiTempSad            = m_cDistParam.distFunc( m_cDistParam );
          uiSad               += uiTempSad >> m_cDistParam.subShift;

          if(((uiSad << isubShift) + uiBitCost) > rcStruct.uiBestSad)
          {
            break;
          }

          m_cDistParam.subShift--;
        }

        if(m_cDistParam.subShift == 0)
        {
          uiSad += uiBitCost;

          if( uiSad < rcStruct.uiBestSad )
          {
            rcStruct.uiBestSad      = uiSad;
            rcStruct.iBestX         = iSearchX;
            rcStruct.iBestY         = iSearchY;
            rcStruct.uiBestDistance = uiDistance;
            rcStruct.uiBestRound    = 0;
            rcStruct.ucPointNr      = ucPointNr;
            m_cDistParam.maximumDistortionForEarlyExit = uiSad;
          }
        }

        // restore org ptr
        m_cDistParam.org.buf  = pOrgCpy;
        m_cDistParam.subShift = subShift;
      }
    }
  }
  else
  {
    uiSad = m_cDistParam.distFunc( m_cDistParam );

    // only add motion cost if uiSad is smaller than best. Otherwise pointless
    // to add motion cost.
    if( uiSad < rcStruct.uiBestSad )
    {
      // motion cost
      uiSad += m_pcRdCost->getCostOfVectorWithPredictor( iSearchX, iSearchY, rcStruct.imvShift );

      if( uiSad < rcStruct.uiBestSad )
      {
        rcStruct.uiBestSad      = uiSad;
        rcStruct.iBestX         = iSearchX;
        rcStruct.iBestY         = iSearchY;
        rcStruct.uiBestDistance = uiDistance;
        rcStruct.uiBestRound    = 0;
        rcStruct.ucPointNr      = ucPointNr;
        m_cDistParam.maximumDistortionForEarlyExit = uiSad;
      }
    }
  }

//RAMIRO: fim do timer do TZSearchHelp
fimClockTZSearchHelp = clock();
totalClockTZSearchHelp += (fimClockTZSearchHelp - iniClockTZSearchHelp);
}


//RAMIRO: Função xTZ2PointSearch
inline void InterSearch::xTZ2PointSearch( IntTZSearchStruct& rcStruct )
{

  //RAMIRO: contar as repetições do TZ2PointSearch
  n_TZ2PointSearch++;

    //RAMIRO: inicio do timer do TZ2PointSearch
    clock_t iniClockTZ2PointSearch, fimClockTZ2PointSearch;
    iniClockTZ2PointSearch = clock();
 
  const SearchRange& sr = rcStruct.searchRange;

  static const int xOffset[2][9] = { {  0, -1, -1,  0, -1, +1, -1, -1, +1 }, {  0,  0, +1, +1, -1, +1,  0, +1,  0 } };
  static const int yOffset[2][9] = { {  0,  0, -1, -1, +1, -1,  0, +1,  0 }, {  0, -1, -1,  0, -1, +1, +1, +1, +1 } };

  // 2 point search,                   //   1 2 3
  // check only the 2 untested points  //   4 0 5
  // around the start point            //   6 7 8
  const int iX1 = rcStruct.iBestX + xOffset[0][rcStruct.ucPointNr];
  const int iX2 = rcStruct.iBestX + xOffset[1][rcStruct.ucPointNr];

  const int iY1 = rcStruct.iBestY + yOffset[0][rcStruct.ucPointNr];
  const int iY2 = rcStruct.iBestY + yOffset[1][rcStruct.ucPointNr];

  if( iX1 >= sr.left && iX1 <= sr.right && iY1 >= sr.top && iY1 <= sr.bottom )
  {
    xTZSearchHelp( rcStruct, iX1, iY1, 0, 2 );
  }

  if( iX2 >= sr.left && iX2 <= sr.right && iY2 >= sr.top && iY2 <= sr.bottom )
  {
    xTZSearchHelp( rcStruct, iX2, iY2, 0, 2 );
  }

//RAMIRO: fim do timer do TZ2PointSearch
fimClockTZ2PointSearch = clock();
totalClockTZ2PointSearch += (fimClockTZ2PointSearch - iniClockTZ2PointSearch);
}


inline void InterSearch::xTZ8PointSquareSearch( IntTZSearchStruct& rcStruct, const int iStartX, const int iStartY, const int iDist )
{
  const SearchRange& sr = rcStruct.searchRange;
  // 8 point search,                   //   1 2 3
  // search around the start point     //   4 0 5
  // with the required  distance       //   6 7 8
  CHECK( iDist == 0 , "Invalid distance");
  const int iTop        = iStartY - iDist;
  const int iBottom     = iStartY + iDist;
  const int iLeft       = iStartX - iDist;
  const int iRight      = iStartX + iDist;
  rcStruct.uiBestRound += 1;

  if ( iTop >= sr.top ) // check top
  {
    if ( iLeft >= sr.left ) // check top left
    {
      xTZSearchHelp( rcStruct, iLeft, iTop, 1, iDist );
    }
    // top middle
    xTZSearchHelp( rcStruct, iStartX, iTop, 2, iDist );

    if ( iRight <= sr.right ) // check top right
    {
      xTZSearchHelp( rcStruct, iRight, iTop, 3, iDist );
    }
  } // check top
  if ( iLeft >= sr.left ) // check middle left
  {
    xTZSearchHelp( rcStruct, iLeft, iStartY, 4, iDist );
  }
  if ( iRight <= sr.right ) // check middle right
  {
    xTZSearchHelp( rcStruct, iRight, iStartY, 5, iDist );
  }
  if ( iBottom <= sr.bottom ) // check bottom
  {
    if ( iLeft >= sr.left ) // check bottom left
    {
      xTZSearchHelp( rcStruct, iLeft, iBottom, 6, iDist );
    }
    // check bottom middle
    xTZSearchHelp( rcStruct, iStartX, iBottom, 7, iDist );

    if ( iRight <= sr.right ) // check bottom right
    {
      xTZSearchHelp( rcStruct, iRight, iBottom, 8, iDist );
    }
  } // check bottom
}



//RAMIRO: Função xTZ8PointDiamondSearch
inline void InterSearch::xTZ8PointDiamondSearch( IntTZSearchStruct& rcStruct,
                                                 const int iStartX,
                                                 const int iStartY,
                                                 const int iDist,
                                                 const bool bCheckCornersAtDist1 )
{

  //RAMIRO: contar as repetições do TZ8PointDiamondSearch
  n_TZ8PointDiamondSearch++;

    //RAMIRO: inicio do timer do TZ8PointDiamondSearch
    clock_t iniClockTZ8PointDiamondSearch, fimClockTZ8PointDiamondSearch;
    iniClockTZ8PointDiamondSearch = clock();

  const SearchRange& sr = rcStruct.searchRange;
  // 8 point search,                   //   1 2 3
  // search around the start point     //   4 0 5
  // with the required  distance       //   6 7 8
  CHECK( iDist == 0, "Invalid distance" );
  const int iTop        = iStartY - iDist;
  const int iBottom     = iStartY + iDist;
  const int iLeft       = iStartX - iDist;
  const int iRight      = iStartX + iDist;
  rcStruct.uiBestRound += 1;

  if ( iDist == 1 )
  {
    if ( iTop >= sr.top ) // check top
    {
      if (bCheckCornersAtDist1)
      {
        if ( iLeft >= sr.left) // check top-left
        {
          xTZSearchHelp( rcStruct, iLeft, iTop, 1, iDist );
        }
        xTZSearchHelp( rcStruct, iStartX, iTop, 2, iDist );
        if ( iRight <= sr.right ) // check middle right
        {
          xTZSearchHelp( rcStruct, iRight, iTop, 3, iDist );
        }
      }
      else
      {
        xTZSearchHelp( rcStruct, iStartX, iTop, 2, iDist );
      }
    }
    if ( iLeft >= sr.left ) // check middle left
    {
      xTZSearchHelp( rcStruct, iLeft, iStartY, 4, iDist );
    }
    if ( iRight <= sr.right ) // check middle right
    {
      xTZSearchHelp( rcStruct, iRight, iStartY, 5, iDist );
    }
    if ( iBottom <= sr.bottom ) // check bottom
    {
      if (bCheckCornersAtDist1)
      {
        if ( iLeft >= sr.left) // check top-left
        {
          xTZSearchHelp( rcStruct, iLeft, iBottom, 6, iDist );
        }
        xTZSearchHelp( rcStruct, iStartX, iBottom, 7, iDist );
        if ( iRight <= sr.right ) // check middle right
        {
          xTZSearchHelp( rcStruct, iRight, iBottom, 8, iDist );
        }
      }
      else
      {
        xTZSearchHelp( rcStruct, iStartX, iBottom, 7, iDist );
      }
    }
  }
  else
  {
    if ( iDist <= 8 )
    {
      const int iTop_2      = iStartY - (iDist>>1);
      const int iBottom_2   = iStartY + (iDist>>1);
      const int iLeft_2     = iStartX - (iDist>>1);
      const int iRight_2    = iStartX + (iDist>>1);

      if (  iTop >= sr.top && iLeft >= sr.left &&
           iRight <= sr.right && iBottom <= sr.bottom ) // check border
      {
        xTZSearchHelp( rcStruct, iStartX,  iTop,      2, iDist    );
        xTZSearchHelp( rcStruct, iLeft_2,  iTop_2,    1, iDist>>1 );
        xTZSearchHelp( rcStruct, iRight_2, iTop_2,    3, iDist>>1 );
        xTZSearchHelp( rcStruct, iLeft,    iStartY,   4, iDist    );
        xTZSearchHelp( rcStruct, iRight,   iStartY,   5, iDist    );
        xTZSearchHelp( rcStruct, iLeft_2,  iBottom_2, 6, iDist>>1 );
        xTZSearchHelp( rcStruct, iRight_2, iBottom_2, 8, iDist>>1 );
        xTZSearchHelp( rcStruct, iStartX,  iBottom,   7, iDist    );
      }
      else // check border
      {
        if ( iTop >= sr.top ) // check top
        {
          xTZSearchHelp( rcStruct, iStartX, iTop, 2, iDist );
        }
        if ( iTop_2 >= sr.top ) // check half top
        {
          if ( iLeft_2 >= sr.left ) // check half left
          {
            xTZSearchHelp( rcStruct, iLeft_2, iTop_2, 1, (iDist>>1) );
          }
          if ( iRight_2 <= sr.right ) // check half right
          {
            xTZSearchHelp( rcStruct, iRight_2, iTop_2, 3, (iDist>>1) );
          }
        } // check half top
        if ( iLeft >= sr.left ) // check left
        {
          xTZSearchHelp( rcStruct, iLeft, iStartY, 4, iDist );
        }
        if ( iRight <= sr.right ) // check right
        {
          xTZSearchHelp( rcStruct, iRight, iStartY, 5, iDist );
        }
        if ( iBottom_2 <= sr.bottom ) // check half bottom
        {
          if ( iLeft_2 >= sr.left ) // check half left
          {
            xTZSearchHelp( rcStruct, iLeft_2, iBottom_2, 6, (iDist>>1) );
          }
          if ( iRight_2 <= sr.right ) // check half right
          {
            xTZSearchHelp( rcStruct, iRight_2, iBottom_2, 8, (iDist>>1) );
          }
        } // check half bottom
        if ( iBottom <= sr.bottom ) // check bottom
        {
          xTZSearchHelp( rcStruct, iStartX, iBottom, 7, iDist );
        }
      } // check border
    }
    else // iDist > 8
    {
      if ( iTop >= sr.top && iLeft >= sr.left &&
           iRight <= sr.right && iBottom <= sr.bottom ) // check border
      {
        xTZSearchHelp( rcStruct, iStartX, iTop,    0, iDist );
        xTZSearchHelp( rcStruct, iLeft,   iStartY, 0, iDist );
        xTZSearchHelp( rcStruct, iRight,  iStartY, 0, iDist );
        xTZSearchHelp( rcStruct, iStartX, iBottom, 0, iDist );
        for ( int index = 1; index < 4; index++ )
        {
          const int iPosYT = iTop    + ((iDist>>2) * index);
          const int iPosYB = iBottom - ((iDist>>2) * index);
          const int iPosXL = iStartX - ((iDist>>2) * index);
          const int iPosXR = iStartX + ((iDist>>2) * index);
          xTZSearchHelp( rcStruct, iPosXL, iPosYT, 0, iDist );
          xTZSearchHelp( rcStruct, iPosXR, iPosYT, 0, iDist );
          xTZSearchHelp( rcStruct, iPosXL, iPosYB, 0, iDist );
          xTZSearchHelp( rcStruct, iPosXR, iPosYB, 0, iDist );
        }
      }
      else // check border
      {
        if ( iTop >= sr.top ) // check top
        {
          xTZSearchHelp( rcStruct, iStartX, iTop, 0, iDist );
        }
        if ( iLeft >= sr.left ) // check left
        {
          xTZSearchHelp( rcStruct, iLeft, iStartY, 0, iDist );
        }
        if ( iRight <= sr.right ) // check right
        {
          xTZSearchHelp( rcStruct, iRight, iStartY, 0, iDist );
        }
        if ( iBottom <= sr.bottom ) // check bottom
        {
          xTZSearchHelp( rcStruct, iStartX, iBottom, 0, iDist );
        }
        for ( int index = 1; index < 4; index++ )
        {
          const int iPosYT = iTop    + ((iDist>>2) * index);
          const int iPosYB = iBottom - ((iDist>>2) * index);
          const int iPosXL = iStartX - ((iDist>>2) * index);
          const int iPosXR = iStartX + ((iDist>>2) * index);

          if ( iPosYT >= sr.top ) // check top
          {
            if ( iPosXL >= sr.left ) // check left
            {
              xTZSearchHelp( rcStruct, iPosXL, iPosYT, 0, iDist );
            }
            if ( iPosXR <= sr.right ) // check right
            {
              xTZSearchHelp( rcStruct, iPosXR, iPosYT, 0, iDist );
            }
          } // check top
          if ( iPosYB <= sr.bottom ) // check bottom
          {
            if ( iPosXL >= sr.left ) // check left
            {
              xTZSearchHelp( rcStruct, iPosXL, iPosYB, 0, iDist );
            }
            if ( iPosXR <= sr.right ) // check right
            {
              xTZSearchHelp( rcStruct, iPosXR, iPosYB, 0, iDist );
            }
          } // check bottom
        } // for ...
      } // check border
    } // iDist <= 8
  } // iDist == 1

//RAMIRO: fim do timer do TZ8PointDiamondSearch
fimClockTZ8PointDiamondSearch = clock();
totalClockTZ8PointDiamondSearch += (fimClockTZ8PointDiamondSearch - iniClockTZ8PointDiamondSearch);
}

#if GDR_ENABLED
Distortion InterSearch::xPatternRefinement(const PredictionUnit &pu, RefPicList eRefPicList, int refIdx,
                                           const CPelBuf *pcPatternKey, Mv baseRefMv, int iFrac, Mv &rcMvFrac,
                                           bool bAllowUseOfHadamard, bool &rbCleanCandExist)
#else

Distortion InterSearch::xPatternRefinement( const CPelBuf* pcPatternKey,
                                            Mv baseRefMv,
                                            int iFrac, Mv& rcMvFrac,
                                            bool bAllowUseOfHadamard )
#endif
{
  Distortion dist;
  Distortion distBest   = std::numeric_limits<Distortion>::max();
  uint32_t   directBest = 0;

#if GDR_ENABLED
  const CodingStructure &cs = *pu.cs;
  const bool isEncodeGdrClean = cs.sps->getGDREnabledFlag() && cs.pcv->isEncoder && ((cs.picHeader->getInGdrInterval() && cs.isClean(pu.Y().topRight(), CHANNEL_TYPE_LUMA)) || (cs.picHeader->getNumVerVirtualBoundaries() == 0));
  bool                   diskOk           = false;
  bool                   distBestOk       = false;
  bool allOk = true;
#endif
  Pel*  piRefPos;
  int iRefStride = pcPatternKey->width + 1;
  m_pcRdCost->setDistParam( m_cDistParam, *pcPatternKey, m_filteredBlock[0][0][0], iRefStride, m_lumaClpRng.bd, COMPONENT_Y, 0, 1, m_pcEncCfg->getUseHADME() && bAllowUseOfHadamard );

  const Mv* pcMvRefine = (iFrac == 2 ? s_acMvRefineH : s_acMvRefineQ);
#if GDR_ENABLED
  if (isEncodeGdrClean)
  {
    rbCleanCandExist = false;
  }
#endif
  for (uint32_t i = 0; i < 9; i++)
  {
    if (m_skipFracME && i > 0)
    {
      break;
    }
    Mv cMvTest = pcMvRefine[i];
    cMvTest += baseRefMv;

    int horVal = cMvTest.getHor() * iFrac;
    int verVal = cMvTest.getVer() * iFrac;
    piRefPos = m_filteredBlock[verVal & 3][horVal & 3][0];

    if (horVal == 2 && (verVal & 1) == 0)
    {
      piRefPos += 1;
    }
    if ((horVal & 1) == 0 && verVal == 2)
    {
      piRefPos += iRefStride;
    }
    cMvTest = pcMvRefine[i];
    cMvTest += rcMvFrac;


    m_cDistParam.cur.buf   = piRefPos;
    dist                   = m_cDistParam.distFunc(m_cDistParam);
    dist += m_pcRdCost->getCostOfVectorWithPredictor(cMvTest.getHor(), cMvTest.getVer(), 0);

#if GDR_ENABLED
    allOk = (dist < distBest);

    if (isEncodeGdrClean)
    {
      Mv motion = cMvTest;
      MvPrecision curPrec = (iFrac == 2 ? MV_PRECISION_HALF : MV_PRECISION_QUARTER);
      motion.changePrecision(curPrec, MV_PRECISION_INTERNAL);
      diskOk = cs.isClean(pu.Y().bottomRight(), motion, eRefPicList, refIdx);

      if (diskOk)
      {
        allOk = (distBestOk) ? (dist < distBest) : true;
      }
      else
      {
        allOk = false;
      }
    }
#endif

#if GDR_ENABLED
    if (allOk)
#else
    if (dist < distBest)
#endif
    {
      distBest                                   = dist;
      directBest                                 = i;
      m_cDistParam.maximumDistortionForEarlyExit = dist;
#if GDR_ENABLED
      if (isEncodeGdrClean)
      {
        distBestOk       = diskOk;
        rbCleanCandExist = true;
      }
#endif
    }
#if GDR_ENABLED
    if (isEncodeGdrClean)
    {
      if (!rbCleanCandExist)
        distBest = 65535;
    }
#endif
  }

  rcMvFrac = pcMvRefine[directBest];

  return distBest;
}

Distortion InterSearch::xGetInterPredictionError( PredictionUnit& pu, PelUnitBuf& origBuf, const RefPicList &eRefPicList )
{
  PelUnitBuf predBuf = m_tmpStorageLCU.getBuf( UnitAreaRelative(*pu.cu, pu) );

  motionCompensation( pu, predBuf, eRefPicList );

  DistParam cDistParam;
  cDistParam.applyWeight = false;

  m_pcRdCost->setDistParam(cDistParam, origBuf.Y(), predBuf.Y(), pu.cs->sps->getBitDepth(CHANNEL_TYPE_LUMA), COMPONENT_Y, m_pcEncCfg->getUseHADME() && !pu.cu->slice->getDisableSATDForRD());

  return (Distortion)cDistParam.distFunc( cDistParam );
}

/// add ibc search functions here

void InterSearch::xIBCSearchMVCandUpdate(Distortion  sad, int x, int y, Distortion* sadBestCand, Mv* cMVCand)
{
  int j = CHROMA_REFINEMENT_CANDIDATES - 1;

  if (sad < sadBestCand[CHROMA_REFINEMENT_CANDIDATES - 1])
  {
    for (int t = CHROMA_REFINEMENT_CANDIDATES - 1; t >= 0; t--)
    {
      if (sad < sadBestCand[t])
      {
        j = t;
      }
    }

    for (int k = CHROMA_REFINEMENT_CANDIDATES - 1; k > j; k--)
    {
      sadBestCand[k] = sadBestCand[k - 1];

      cMVCand[k].set(cMVCand[k - 1].getHor(), cMVCand[k - 1].getVer());
    }
    sadBestCand[j] = sad;
    cMVCand[j].set(x, y);
  }
}

int InterSearch::xIBCSearchMVChromaRefine(PredictionUnit& pu,
  int         roiWidth,
  int         roiHeight,
  int         cuPelX,
  int         cuPelY,
  Distortion* sadBestCand,
  Mv*     cMVCand

)
{
  if ( (!isChromaEnabled(pu.chromaFormat)) || (!pu.Cb().valid()) )
  {
    return 0;
  }

  int bestCandIdx = 0;
  Distortion  sadBest = std::numeric_limits<Distortion>::max();
  Distortion  tempSad;

  Pel* pRef;
  Pel* pOrg;
  int refStride, orgStride;
  int width, height;

  int picWidth = pu.cs->slice->getPPS()->getPicWidthInLumaSamples();
  int picHeight = pu.cs->slice->getPPS()->getPicHeightInLumaSamples();

  UnitArea allCompBlocks(pu.chromaFormat, (Area)pu.block(COMPONENT_Y));
  for (int cand = 0; cand < CHROMA_REFINEMENT_CANDIDATES; cand++)
  {
    if (sadBestCand[cand] == std::numeric_limits<Distortion>::max())
    {
      continue;
    }

    if ((!cMVCand[cand].getHor()) && (!cMVCand[cand].getVer()))
    {
      continue;
    }

    if (((int)(cuPelY + cMVCand[cand].getVer() + roiHeight) >= picHeight) || ((cuPelY + cMVCand[cand].getVer()) < 0))
    {
      continue;
    }

    if (((int)(cuPelX + cMVCand[cand].getHor() + roiWidth) >= picWidth) || ((cuPelX + cMVCand[cand].getHor()) < 0))
    {
      continue;
    }

#if GDR_ENABLED
    CodingStructure &cs = *pu.cs;
    const bool isEncodeGdrClean = cs.sps->getGDREnabledFlag() && cs.pcv->isEncoder && ((cs.picHeader->getInGdrInterval() && cs.isClean(pu.Y().topRight(), CHANNEL_TYPE_LUMA)) || (cs.picHeader->getNumVerVirtualBoundaries() == 0));

    if (isEncodeGdrClean)
    {
      Position curBR(cuPelX + roiWidth + cMVCand[cand].getHor() - 1, cuPelY + roiHeight + cMVCand[cand].getVer() - 1);    // is this correct???
      if (!cs.isClean(curBR, CHANNEL_TYPE_LUMA))
      {
        continue;
      }
    }
#endif

    tempSad = sadBestCand[cand];

    pu.mv[0] = cMVCand[cand];
    pu.mv[0].changePrecision(MV_PRECISION_INT, MV_PRECISION_INTERNAL);
    pu.interDir = 1;
    pu.refIdx[0] = pu.cs->slice->getNumRefIdx(REF_PIC_LIST_0); // last idx in the list

    PelUnitBuf predBufTmp = m_tmpPredStorage[REF_PIC_LIST_0].getBuf(UnitAreaRelative(*pu.cu, pu));
    motionCompensation(pu, predBufTmp, REF_PIC_LIST_0);

    for (unsigned int ch = COMPONENT_Cb; ch < ::getNumberValidComponents(pu.chromaFormat); ch++)
    {
      width = roiWidth >> ::getComponentScaleX(ComponentID(ch), pu.chromaFormat);
      height = roiHeight >> ::getComponentScaleY(ComponentID(ch), pu.chromaFormat);

      PelUnitBuf origBuf = pu.cs->getOrgBuf(allCompBlocks);
      PelUnitBuf* pBuf = &origBuf;
      CPelBuf  tmpPattern = pBuf->get(ComponentID(ch));
      pOrg = (Pel*)tmpPattern.buf;

      Picture* refPic = pu.cu->slice->getPic();
      const CPelBuf refBuf = refPic->getRecoBuf(allCompBlocks.blocks[ComponentID(ch)]);
      pRef = (Pel*)refBuf.buf;

      refStride = refBuf.stride;
      orgStride = tmpPattern.stride;

      //ComponentID compID = (ComponentID)ch;
      PelUnitBuf* pBufRef = &predBufTmp;
      CPelBuf  tmpPatternRef = pBufRef->get(ComponentID(ch));
      pRef = (Pel*)tmpPatternRef.buf;
      refStride = tmpPatternRef.stride;


      for (int row = 0; row < height; row++)
      {
        for (int col = 0; col < width; col++)
        {
          tempSad += ((abs(pRef[col] - pOrg[col])) >> (pu.cs->sps->getBitDepth(CHANNEL_TYPE_CHROMA) - 8));
        }
        pRef += refStride;
        pOrg += orgStride;
      }
    }

    if (tempSad < sadBest)
    {
      sadBest = tempSad;
      bestCandIdx = cand;
    }
  }

  return bestCandIdx;
}

static unsigned int xMergeCandLists(Mv *dst, unsigned int dn, unsigned int dstTotalLength, Mv *src, unsigned int sn)
{
  for (unsigned int cand = 0; cand < sn && dn < dstTotalLength; cand++)
  {
    if (src[cand] == Mv())
    {
      continue;
    }
    bool found = false;
    for (int j = 0; j<dn; j++)
    {
      if (src[cand] == dst[j])
      {
        found = true;
        break;
      }
    }

    if (!found)
    {
      dst[dn] = src[cand];
      dn++;
    }
  }

  return dn;
}

void InterSearch::xIntraPatternSearch(PredictionUnit& pu, IntTZSearchStruct&  cStruct, Mv& rcMv, Distortion&  ruiCost, Mv*  pcMvSrchRngLT, Mv*  pcMvSrchRngRB, Mv* pcMvPred)
{
  const int   srchRngHorLeft = pcMvSrchRngLT->getHor();
  const int   srchRngHorRight = pcMvSrchRngRB->getHor();
  const int   srchRngVerTop = pcMvSrchRngLT->getVer();
  const int   srchRngVerBottom = pcMvSrchRngRB->getVer();

  const unsigned int  lcuWidth = pu.cs->slice->getSPS()->getMaxCUWidth();
  const int   puPelOffsetX = 0;
  const int   puPelOffsetY = 0;
  const int   cuPelX = pu.Y().x;
  const int   cuPelY = pu.Y().y;

  int          roiWidth = pu.lwidth();
  int          roiHeight = pu.lheight();

  Distortion  sad;
  Distortion  sadBest = std::numeric_limits<Distortion>::max();
  int         bestX = 0;
  int         bestY = 0;

  const Pel*        piRefSrch = cStruct.piRefY;

  int         bestCandIdx = 0;

  Distortion  sadBestCand[CHROMA_REFINEMENT_CANDIDATES];
  Mv      cMVCand[CHROMA_REFINEMENT_CANDIDATES];

#if GDR_ENABLED
  CodingStructure &cs = *pu.cs;
  const bool isEncodeGdrClean = cs.sps->getGDREnabledFlag() && cs.pcv->isEncoder && ((cs.picHeader->getInGdrInterval() && cs.isClean(pu.Y().topRight(), CHANNEL_TYPE_LUMA)) || (cs.picHeader->getNumVerVirtualBoundaries() == 0));
#endif

  for (int cand = 0; cand < CHROMA_REFINEMENT_CANDIDATES; cand++)
  {
    sadBestCand[cand] = std::numeric_limits<Distortion>::max();
    cMVCand[cand].set(0, 0);
  }

  m_cDistParam.useMR = false;
  m_pcRdCost->setDistParam(m_cDistParam, *cStruct.pcPatternKey, cStruct.piRefY, cStruct.iRefStride, m_lumaClpRng.bd, COMPONENT_Y, cStruct.subShiftMode);

  const int picWidth = pu.cs->slice->getPPS()->getPicWidthInLumaSamples();
  const int picHeight = pu.cs->slice->getPPS()->getPicHeightInLumaSamples();


  {
    m_cDistParam.subShift = 0;

    Distortion tempSadBest = 0;

    int srLeft = srchRngHorLeft, srRight = srchRngHorRight, srTop = srchRngVerTop, srBottom = srchRngVerBottom;
    m_numBVs = 0;
    m_numBVs = xMergeCandLists(m_acBVs, m_numBVs, (2 * IBC_NUM_CANDIDATES), m_defaultCachedBvs.m_bvCands, m_defaultCachedBvs.currCnt);

    Mv cMvPredEncOnly[IBC_NUM_CANDIDATES];
    int nbPreds = 0;
    PU::getIbcMVPsEncOnly(pu, cMvPredEncOnly, nbPreds);
    m_numBVs = xMergeCandLists(m_acBVs, m_numBVs, (2 * IBC_NUM_CANDIDATES), cMvPredEncOnly, nbPreds);

    for (unsigned int cand = 0; cand < m_numBVs; cand++)
    {
      int xPred = m_acBVs[cand].getHor();
      int yPred = m_acBVs[cand].getVer();

      if (!(xPred == 0 && yPred == 0)
        && !((yPred < srTop) || (yPred > srBottom))
        && !((xPred < srLeft) || (xPred > srRight)))
      {
        bool validCand = searchBv(pu, cuPelX, cuPelY, roiWidth, roiHeight, picWidth, picHeight, xPred, yPred, lcuWidth);
#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          Position BvBR(cuPelX + roiWidth + xPred - 1, cuPelY + roiHeight + yPred - 1);
          validCand = validCand && cs.isClean(BvBR, CHANNEL_TYPE_LUMA);
        }
#endif
        if (validCand)
        {
          sad = m_pcRdCost->getBvCostMultiplePreds(xPred, yPred, pu.cs->sps->getAMVREnabledFlag());
          m_cDistParam.cur.buf = piRefSrch + cStruct.iRefStride * yPred + xPred;
          sad += m_cDistParam.distFunc(m_cDistParam);

          xIBCSearchMVCandUpdate(sad, xPred, yPred, sadBestCand, cMVCand);
        }
      }
    }

    bestX = cMVCand[0].getHor();
    bestY = cMVCand[0].getVer();
    rcMv.set(bestX, bestY);
    sadBest = sadBestCand[0];

    const int boundY = (0 - roiHeight - puPelOffsetY);
    for (int y = std::max(srchRngVerTop, 0 - cuPelY); y <= boundY; ++y)
    {
      if (!searchBv(pu, cuPelX, cuPelY, roiWidth, roiHeight, picWidth, picHeight, 0, y, lcuWidth))
      {
        continue;
      }
#if GDR_ENABLED
      if (isEncodeGdrClean)
      {
        Position BvBR(cuPelX + roiWidth - 1, cuPelY + roiHeight + y - 1);
        if (!cs.isClean(BvBR, CHANNEL_TYPE_LUMA))
        {
          continue;
        }
      }
#endif

      sad = m_pcRdCost->getBvCostMultiplePreds(0, y, pu.cs->sps->getAMVREnabledFlag());
      m_cDistParam.cur.buf = piRefSrch + cStruct.iRefStride * y;
      sad += m_cDistParam.distFunc(m_cDistParam);

      xIBCSearchMVCandUpdate(sad, 0, y, sadBestCand, cMVCand);
      tempSadBest = sadBestCand[0];
      if (sadBestCand[0] <= 3)
      {
        bestX = cMVCand[0].getHor();
        bestY = cMVCand[0].getVer();
        sadBest = sadBestCand[0];
        rcMv.set(bestX, bestY);
        ruiCost = sadBest;
        goto end;
      }
    }

    const int boundX = std::max(srchRngHorLeft, -cuPelX);
    for (int x = 0 - roiWidth - puPelOffsetX; x >= boundX; --x)
    {
      if (!searchBv(pu, cuPelX, cuPelY, roiWidth, roiHeight, picWidth, picHeight, x, 0, lcuWidth))
      {
        continue;
      }
#if GDR_ENABLED
      if (isEncodeGdrClean)
      {
        Position BvBR(cuPelX + roiWidth + x - 1, cuPelY + roiHeight - 1);
        if (!cs.isClean(BvBR, CHANNEL_TYPE_LUMA))
        {
          continue;
        }
      }
#endif

      sad = m_pcRdCost->getBvCostMultiplePreds(x, 0, pu.cs->sps->getAMVREnabledFlag());
      m_cDistParam.cur.buf = piRefSrch + x;
      sad += m_cDistParam.distFunc(m_cDistParam);


      xIBCSearchMVCandUpdate(sad, x, 0, sadBestCand, cMVCand);
      tempSadBest = sadBestCand[0];
      if (sadBestCand[0] <= 3)
      {
        bestX = cMVCand[0].getHor();
        bestY = cMVCand[0].getVer();
        sadBest = sadBestCand[0];
        rcMv.set(bestX, bestY);
        ruiCost = sadBest;
        goto end;
      }
    }

    bestX = cMVCand[0].getHor();
    bestY = cMVCand[0].getVer();
    sadBest = sadBestCand[0];
    if ((!bestX && !bestY) || (sadBest - m_pcRdCost->getBvCostMultiplePreds(bestX, bestY, pu.cs->sps->getAMVREnabledFlag()) <= 32))
    {
      //chroma refine
      bestCandIdx = xIBCSearchMVChromaRefine(pu, roiWidth, roiHeight, cuPelX, cuPelY, sadBestCand, cMVCand);
      bestX = cMVCand[bestCandIdx].getHor();
      bestY = cMVCand[bestCandIdx].getVer();
      sadBest = sadBestCand[bestCandIdx];
      rcMv.set(bestX, bestY);
      ruiCost = sadBest;
      goto end;
    }


    if (pu.lwidth() < 16 && pu.lheight() < 16)
    {
      for (int y = std::max(srchRngVerTop, -cuPelY); y <= srchRngVerBottom; y += 2)
      {
        if ((y == 0) || ((int)(cuPelY + y + roiHeight) >= picHeight))
        {
          continue;
        }

        for (int x = std::max(srchRngHorLeft, -cuPelX); x <= srchRngHorRight; x++)
        {
          if ((x == 0) || ((int)(cuPelX + x + roiWidth) >= picWidth))
          {
            continue;
          }

          if (!searchBv(pu, cuPelX, cuPelY, roiWidth, roiHeight, picWidth, picHeight, x, y, lcuWidth))
          {
            continue;
          }

#if GDR_ENABLED
          if (isEncodeGdrClean)
          {
            Position BvBR(cuPelX + roiWidth + x - 1, cuPelY + roiHeight + y - 1);
            if (!cs.isClean(BvBR, CHANNEL_TYPE_LUMA))
            {
              continue;
            }
          }
#endif

          sad = m_pcRdCost->getBvCostMultiplePreds(x, y, pu.cs->sps->getAMVREnabledFlag());
          m_cDistParam.cur.buf = piRefSrch + cStruct.iRefStride * y + x;
          sad += m_cDistParam.distFunc(m_cDistParam);

          xIBCSearchMVCandUpdate(sad, x, y, sadBestCand, cMVCand);
        }
      }

      bestX = cMVCand[0].getHor();
      bestY = cMVCand[0].getVer();
      sadBest = sadBestCand[0];
      if (sadBest - m_pcRdCost->getBvCostMultiplePreds(bestX, bestY, pu.cs->sps->getAMVREnabledFlag()) <= 16)
      {
        //chroma refine
        bestCandIdx = xIBCSearchMVChromaRefine(pu, roiWidth, roiHeight, cuPelX, cuPelY, sadBestCand, cMVCand);

        bestX = cMVCand[bestCandIdx].getHor();
        bestY = cMVCand[bestCandIdx].getVer();
        sadBest = sadBestCand[bestCandIdx];
        rcMv.set(bestX, bestY);
        ruiCost = sadBest;
        goto end;
      }


      for (int y = (std::max(srchRngVerTop, -cuPelY) + 1); y <= srchRngVerBottom; y += 2)
      {
        if ((y == 0) || ((int)(cuPelY + y + roiHeight) >= picHeight))
        {
          continue;
        }

        for (int x = std::max(srchRngHorLeft, -cuPelX); x <= srchRngHorRight; x += 2)
        {
          if ((x == 0) || ((int)(cuPelX + x + roiWidth) >= picWidth))
          {
            continue;
          }

          if (!searchBv(pu, cuPelX, cuPelY, roiWidth, roiHeight, picWidth, picHeight, x, y, lcuWidth))
          {
            continue;
          }

#if GDR_ENABLED
          if (isEncodeGdrClean)
          {
            Position BvBR(cuPelX + roiWidth + x - 1, cuPelY + roiHeight + y - 1);
            if (!cs.isClean(BvBR, CHANNEL_TYPE_LUMA))
            {
              continue;
            }
          }
#endif

          sad = m_pcRdCost->getBvCostMultiplePreds(x, y, pu.cs->sps->getAMVREnabledFlag());
          m_cDistParam.cur.buf = piRefSrch + cStruct.iRefStride * y + x;
          sad += m_cDistParam.distFunc(m_cDistParam);


          xIBCSearchMVCandUpdate(sad, x, y, sadBestCand, cMVCand);
          if (sadBestCand[0] <= 5)
          {
            //chroma refine & return
            bestCandIdx = xIBCSearchMVChromaRefine(pu, roiWidth, roiHeight, cuPelX, cuPelY, sadBestCand, cMVCand);
            bestX = cMVCand[bestCandIdx].getHor();
            bestY = cMVCand[bestCandIdx].getVer();
            sadBest = sadBestCand[bestCandIdx];
            rcMv.set(bestX, bestY);
            ruiCost = sadBest;
            goto end;
          }
        }
      }

      bestX = cMVCand[0].getHor();
      bestY = cMVCand[0].getVer();
      sadBest = sadBestCand[0];

      if ((sadBest >= tempSadBest) || ((sadBest - m_pcRdCost->getBvCostMultiplePreds(bestX, bestY, pu.cs->sps->getAMVREnabledFlag())) <= 32))
      {
        //chroma refine
        bestCandIdx = xIBCSearchMVChromaRefine(pu, roiWidth, roiHeight, cuPelX, cuPelY, sadBestCand, cMVCand);
        bestX = cMVCand[bestCandIdx].getHor();
        bestY = cMVCand[bestCandIdx].getVer();
        sadBest = sadBestCand[bestCandIdx];
        rcMv.set(bestX, bestY);
        ruiCost = sadBest;
        goto end;
      }

      tempSadBest = sadBestCand[0];


      for (int y = (std::max(srchRngVerTop, -cuPelY) + 1); y <= srchRngVerBottom; y += 2)
      {
        if ((y == 0) || ((int)(cuPelY + y + roiHeight) >= picHeight))
        {
          continue;
        }

        for (int x = (std::max(srchRngHorLeft, -cuPelX) + 1); x <= srchRngHorRight; x += 2)
        {

          if ((x == 0) || ((int)(cuPelX + x + roiWidth) >= picWidth))
          {
            continue;
          }

          if (!searchBv(pu, cuPelX, cuPelY, roiWidth, roiHeight, picWidth, picHeight, x, y, lcuWidth))
          {
            continue;
          }
#if GDR_ENABLED
          if (isEncodeGdrClean)
          {
            Position BvBR(cuPelX + roiWidth + x - 1, cuPelY + roiHeight + y - 1);
            if (!cs.isClean(BvBR, CHANNEL_TYPE_LUMA))
            {
              continue;
            }
          }
#endif

          sad = m_pcRdCost->getBvCostMultiplePreds(x, y, pu.cs->sps->getAMVREnabledFlag());
          m_cDistParam.cur.buf = piRefSrch + cStruct.iRefStride * y + x;
          sad += m_cDistParam.distFunc(m_cDistParam);


          xIBCSearchMVCandUpdate(sad, x, y, sadBestCand, cMVCand);
          if (sadBestCand[0] <= 5)
          {
            //chroma refine & return
            bestCandIdx = xIBCSearchMVChromaRefine(pu, roiWidth, roiHeight, cuPelX, cuPelY, sadBestCand, cMVCand);
            bestX = cMVCand[bestCandIdx].getHor();
            bestY = cMVCand[bestCandIdx].getVer();
            sadBest = sadBestCand[bestCandIdx];
            rcMv.set(bestX, bestY);
            ruiCost = sadBest;
            goto end;
          }
        }
      }
    }
  }

  bestCandIdx = xIBCSearchMVChromaRefine(pu, roiWidth, roiHeight, cuPelX, cuPelY, sadBestCand, cMVCand);

  bestX = cMVCand[bestCandIdx].getHor();
  bestY = cMVCand[bestCandIdx].getVer();
  sadBest = sadBestCand[bestCandIdx];
  rcMv.set(bestX, bestY);
  ruiCost = sadBest;

end:
  m_numBVs = 0;
  m_numBVs = xMergeCandLists(m_acBVs, m_numBVs, (2 * IBC_NUM_CANDIDATES), m_defaultCachedBvs.m_bvCands, m_defaultCachedBvs.currCnt);

  m_defaultCachedBvs.currCnt = 0;
  m_defaultCachedBvs.currCnt = xMergeCandLists(m_defaultCachedBvs.m_bvCands, m_defaultCachedBvs.currCnt, IBC_NUM_CANDIDATES, cMVCand, CHROMA_REFINEMENT_CANDIDATES);
  m_defaultCachedBvs.currCnt = xMergeCandLists(m_defaultCachedBvs.m_bvCands, m_defaultCachedBvs.currCnt, IBC_NUM_CANDIDATES, m_acBVs, m_numBVs);

  for (unsigned int cand = 0; cand < CHROMA_REFINEMENT_CANDIDATES; cand++)
  {
    if (cMVCand[cand].getHor() == 0 && cMVCand[cand].getVer() == 0)
    {
      continue;
    }
    m_ctuRecord[pu.lumaPos()][pu.lumaSize()].bvRecord[cMVCand[cand]] = sadBestCand[cand];
  }

  return;
}



// based on xMotionEstimation
void InterSearch::xIBCEstimation(PredictionUnit& pu, PelUnitBuf& origBuf,
  Mv     *pcMvPred,
  Mv     &rcMv,
  Distortion &ruiCost, const int localSearchRangeX, const int localSearchRangeY
)
{
  const int iPicWidth = pu.cs->slice->getPPS()->getPicWidthInLumaSamples();
  const int iPicHeight = pu.cs->slice->getPPS()->getPicHeightInLumaSamples();
  const unsigned int  lcuWidth = pu.cs->slice->getSPS()->getMaxCUWidth();
  const int           cuPelX = pu.Y().x;
  const int           cuPelY = pu.Y().y;
  int                 iRoiWidth = pu.lwidth();
  int                 iRoiHeight = pu.lheight();

  PelUnitBuf* pBuf = &origBuf;

  //  Search key pattern initialization
  CPelBuf  tmpPattern = pBuf->Y();
  CPelBuf* pcPatternKey = &tmpPattern;
  PelBuf tmpOrgLuma;

#if GDR_ENABLED
  CodingStructure &cs = *pu.cs;
  const bool isEncodeGdrClean = cs.sps->getGDREnabledFlag() && cs.pcv->isEncoder && ((cs.picHeader->getInGdrInterval() && cs.isClean(pu.Y().topRight(), CHANNEL_TYPE_LUMA)) || (cs.picHeader->getNumVerVirtualBoundaries() == 0));
#endif

  if ((pu.cs->slice->getLmcsEnabledFlag() && m_pcReshape->getCTUFlag()))
  {
    const CompArea &area = pu.blocks[COMPONENT_Y];
    CompArea    tmpArea(COMPONENT_Y, area.chromaFormat, Position(0, 0), area.size());
    tmpOrgLuma = m_tmpStorageLCU.getBuf(tmpArea);
    tmpOrgLuma.copyFrom(tmpPattern);
    tmpOrgLuma.rspSignal(m_pcReshape->getFwdLUT());
    pcPatternKey = (CPelBuf*)&tmpOrgLuma;
  }

  m_lumaClpRng = pu.cs->slice->clpRng(COMPONENT_Y);
  Picture* refPic = pu.cu->slice->getPic();
  const CPelBuf refBuf = refPic->getRecoBuf(pu.blocks[COMPONENT_Y]);

  IntTZSearchStruct cStruct;
  cStruct.pcPatternKey = pcPatternKey;
  cStruct.iRefStride = refBuf.stride;
  cStruct.piRefY = refBuf.buf;
  CHECK(pu.cu->imv == IMV_HPEL, "IF_IBC");
  cStruct.imvShift = pu.cu->imv << 1;
  cStruct.subShiftMode = 0; // used by intra pattern search function

  // disable weighted prediction
  setWpScalingDistParam(-1, REF_PIC_LIST_X, pu.cs->slice);

  m_pcRdCost->getMotionCost(0);
  m_pcRdCost->setPredictors(pcMvPred);
  m_pcRdCost->setCostScale(0);

  m_cDistParam.useMR = false;
  m_pcRdCost->setDistParam(m_cDistParam, *cStruct.pcPatternKey, cStruct.piRefY, cStruct.iRefStride, m_lumaClpRng.bd, COMPONENT_Y, cStruct.subShiftMode);
  bool buffered = false;
  if (m_pcEncCfg->getIBCFastMethod() & IBC_FAST_METHOD_BUFFERBV)
  {
    ruiCost = MAX_UINT;
    std::unordered_map<Mv, Distortion>& history = m_ctuRecord[pu.lumaPos()][pu.lumaSize()].bvRecord;
    for (std::unordered_map<Mv, Distortion>::iterator p = history.begin(); p != history.end(); p++)
    {
      const Mv& bv = p->first;

      int xBv = bv.hor;
      int yBv = bv.ver;
#if GDR_ENABLED
      bool validCand = true;
      if (isEncodeGdrClean)
      {
        Position BvBR(cuPelX + iRoiWidth + xBv - 1, cuPelY + iRoiHeight + yBv - 1);
        validCand = validCand && cs.isClean(BvBR, CHANNEL_TYPE_LUMA);
      }
      if (validCand && searchBv(pu, cuPelX, cuPelY, iRoiWidth, iRoiHeight, iPicWidth, iPicHeight, xBv, yBv, lcuWidth))
#else
      if (searchBv(pu, cuPelX, cuPelY, iRoiWidth, iRoiHeight, iPicWidth, iPicHeight, xBv, yBv, lcuWidth))
#endif
      {
        buffered = true;
        Distortion sad = m_pcRdCost->getBvCostMultiplePreds(xBv, yBv, pu.cs->sps->getAMVREnabledFlag());
        m_cDistParam.cur.buf = cStruct.piRefY + cStruct.iRefStride * yBv + xBv;
        sad += m_cDistParam.distFunc(m_cDistParam);
        if (sad < ruiCost)
        {
          rcMv = bv;
          ruiCost = sad;
        }
        else if (sad == ruiCost)
        {
          // stabilise the search through the unordered list
          if (bv.hor < rcMv.getHor()
            || (bv.hor == rcMv.getHor() && bv.ver < rcMv.getVer()))
          {
            // update the vector.
            rcMv = bv;
          }
        }
      }
    }

    if (buffered)
    {
      Mv cMvPredEncOnly[IBC_NUM_CANDIDATES];
      int nbPreds = 0;
      PU::getIbcMVPsEncOnly(pu, cMvPredEncOnly, nbPreds);

      for (unsigned int cand = 0; cand < nbPreds; cand++)
      {
        int xPred = cMvPredEncOnly[cand].getHor();
        int yPred = cMvPredEncOnly[cand].getVer();

#if GDR_ENABLED
        bool validCand = true;
        if (isEncodeGdrClean)
        {
          Position BvBR(cuPelX + iRoiWidth + xPred - 1, cuPelY + iRoiHeight + yPred - 1);
          validCand = cs.isClean(BvBR, CHANNEL_TYPE_LUMA);
        }
        if (validCand && searchBv(pu, cuPelX, cuPelY, iRoiWidth, iRoiHeight, iPicWidth, iPicHeight, xPred, yPred, lcuWidth))
#else
        if (searchBv(pu, cuPelX, cuPelY, iRoiWidth, iRoiHeight, iPicWidth, iPicHeight, xPred, yPred, lcuWidth))
#endif
        {
          Distortion sad = m_pcRdCost->getBvCostMultiplePreds(xPred, yPred, pu.cs->sps->getAMVREnabledFlag());
          m_cDistParam.cur.buf = cStruct.piRefY + cStruct.iRefStride * yPred + xPred;
          sad += m_cDistParam.distFunc(m_cDistParam);
          if (sad < ruiCost)
          {
            rcMv.set(xPred, yPred);
            ruiCost = sad;
          }
          else if (sad == ruiCost)
          {
            // stabilise the search through the unordered list
            if (xPred < rcMv.getHor()
              || (xPred == rcMv.getHor() && yPred < rcMv.getVer()))
            {
              // update the vector.
              rcMv.set(xPred, yPred);
            }
          }

          m_ctuRecord[pu.lumaPos()][pu.lumaSize()].bvRecord[Mv(xPred, yPred)] = sad;
        }
      }
    }
  }

  if (!buffered)
  {
    Mv        cMvSrchRngLT;
    Mv        cMvSrchRngRB;

    // assume that intra BV is integer-pel precision
    xSetIntraSearchRange(pu, pu.lwidth(), pu.lheight(), localSearchRangeX, localSearchRangeY, cMvSrchRngLT, cMvSrchRngRB);

    //  Do integer search
    xIntraPatternSearch(pu, cStruct, rcMv, ruiCost, &cMvSrchRngLT, &cMvSrchRngRB, pcMvPred);
  }
}

// based on xSetSearchRange
void InterSearch::xSetIntraSearchRange(PredictionUnit& pu, int iRoiWidth, int iRoiHeight, const int localSearchRangeX, const int localSearchRangeY, Mv& rcMvSrchRngLT, Mv& rcMvSrchRngRB)
{
  const SPS &sps = *pu.cs->sps;

  int srLeft, srRight, srTop, srBottom;

  const int cuPelX = pu.Y().x;
  const int cuPelY = pu.Y().y;

  const int lcuWidth = pu.cs->slice->getSPS()->getMaxCUWidth();
  const int ctuSizeLog2 = floorLog2(lcuWidth);
  int numLeftCTUs = (1 << ((7 - ctuSizeLog2) << 1)) - ((ctuSizeLog2 < 7) ? 1 : 0);

  srLeft = -(numLeftCTUs * lcuWidth + (cuPelX % lcuWidth));
  srTop = -(cuPelY % lcuWidth);

  srRight = lcuWidth - (cuPelX % lcuWidth) - iRoiWidth;
  srBottom = lcuWidth - (cuPelY % lcuWidth) - iRoiHeight;

  rcMvSrchRngLT.setHor(srLeft);
  rcMvSrchRngLT.setVer(srTop);
  rcMvSrchRngRB.setHor(srRight);
  rcMvSrchRngRB.setVer(srBottom);

  rcMvSrchRngLT <<= 2;
  rcMvSrchRngRB <<= 2;
  bool temp = m_clipMvInSubPic;
  m_clipMvInSubPic = true;
  xClipMv(rcMvSrchRngLT, pu.cu->lumaPos(), pu.cu->lumaSize(), sps, *pu.cs->pps);
  xClipMv(rcMvSrchRngRB, pu.cu->lumaPos(), pu.cu->lumaSize(), sps, *pu.cs->pps);
  m_clipMvInSubPic = temp;
  rcMvSrchRngLT >>= 2;
  rcMvSrchRngRB >>= 2;
}

bool InterSearch::predIBCSearch(CodingUnit& cu, Partitioner& partitioner, const int localSearchRangeX, const int localSearchRangeY, IbcHashMap& ibcHashMap)
{
  Mv           cMvSrchRngLT;
  Mv           cMvSrchRngRB;

  Mv           cMv;
  Mv           cMvPred;

  for (auto &pu : CU::traversePUs(cu))
  {
    m_maxCompIDToPred = MAX_NUM_COMPONENT;

    CHECK(pu.cu != &cu, "PU is contained in another CU");
#if GDR_ENABLED
    CodingStructure &cs = *pu.cs;
    const bool isEncodeGdrClean = cs.sps->getGDREnabledFlag() && cs.pcv->isEncoder && ((cs.picHeader->getInGdrInterval() && cs.isClean(pu.Y().topRight(), CHANNEL_TYPE_LUMA)) || (cs.picHeader->getNumVerVirtualBoundaries() == 0));
#endif

#if GDR_ENABLED
    if (isEncodeGdrClean)
    {
      pu.mvSolid[0] = false;
      pu.mvSolid[1] = false;
      pu.mvValid[0] = false;
      pu.mvValid[1] = false;
    }
#endif
    //////////////////////////////////////////////////////////
    /// ibc search
    pu.cu->imv = 2;
    AMVPInfo amvpInfo4Pel;
    PU::fillIBCMvpCand(pu, amvpInfo4Pel);

    pu.cu->imv = 0;// (Int)cu.cs->sps->getUseIMV(); // set as IMV=0 initially
    Mv    cMv, cMvPred[2];
    AMVPInfo amvpInfo;
    PU::fillIBCMvpCand(pu, amvpInfo);
    // store in full pel accuracy, shift before use in search
    cMvPred[0] = amvpInfo.mvCand[0];
    cMvPred[0].changePrecision(MV_PRECISION_INTERNAL, MV_PRECISION_INT);
    cMvPred[1] = amvpInfo.mvCand[1];
    cMvPred[1].changePrecision(MV_PRECISION_INTERNAL, MV_PRECISION_INT);

    int iBvpNum = 2;
    int bvpIdxBest = 0;
    cMv.setZero();
    Distortion cost = 0;
    if (pu.cs->sps->getMaxNumIBCMergeCand() == 1)
    {
      iBvpNum = 1;
      cMvPred[1] = cMvPred[0];
    }

    if (m_pcEncCfg->getIBCHashSearch())
    {
      xxIBCHashSearch(pu, cMvPred, iBvpNum, cMv, bvpIdxBest, ibcHashMap);
    }

    if (cMv.getHor() == 0 && cMv.getVer() == 0)
    {
      // if hash search does not work or is not enabled
      PelUnitBuf origBuf = pu.cs->getOrgBuf(pu);
      xIBCEstimation(pu, origBuf, cMvPred, cMv, cost, localSearchRangeX, localSearchRangeY);
    }

    if (cMv.getHor() == 0 && cMv.getVer() == 0)
    {
      return false;
    }
    /// ibc search
    /////////////////////////////////////////////////////////
    unsigned int bitsBVPBest, bitsBVPTemp;
    bitsBVPBest = MAX_INT;
    m_pcRdCost->setCostScale(0);

    for (int bvpIdxTemp = 0; bvpIdxTemp<iBvpNum; bvpIdxTemp++)
    {
      m_pcRdCost->setPredictor(cMvPred[bvpIdxTemp]);

      bitsBVPTemp = m_pcRdCost->getBitsOfVectorWithPredictor(cMv.getHor(), cMv.getVer(), 0);

      if (bitsBVPTemp < bitsBVPBest)
      {
        bitsBVPBest = bitsBVPTemp;
        bvpIdxBest = bvpIdxTemp;

        if (cu.cs->sps->getAMVREnabledFlag() && cMv != cMvPred[bvpIdxTemp])
        {
          pu.cu->imv = 1; // set as full-pel
        }
        else
        {
          pu.cu->imv = 0; // set as fractional-pel
        }
      }

      unsigned int bitsBVPQP = MAX_UINT;


      Mv mvPredQuadPel;
      if ((cMv.getHor() % 4 == 0) && (cMv.getVer() % 4 == 0) && (pu.cs->sps->getAMVREnabledFlag()))
      {
        mvPredQuadPel = amvpInfo4Pel.mvCand[bvpIdxTemp];// cMvPred[bvpIdxTemp];

        mvPredQuadPel.changePrecision(MV_PRECISION_INTERNAL, MV_PRECISION_4PEL);

        m_pcRdCost->setPredictor(mvPredQuadPel);

        bitsBVPQP = m_pcRdCost->getBitsOfVectorWithPredictor(cMv.getHor() >> 2, cMv.getVer() >> 2, 0);

      }
      mvPredQuadPel.changePrecision(MV_PRECISION_4PEL, MV_PRECISION_INT);
      if (bitsBVPQP < bitsBVPBest && cMv != mvPredQuadPel)
      {
        bitsBVPBest = bitsBVPQP;
        bvpIdxBest = bvpIdxTemp;

        if (cu.cs->sps->getAMVREnabledFlag())
        {
          pu.cu->imv = 2; // set as quad-pel
        }
      }
    }

    pu.bv = cMv; // bv is always at integer accuracy
    cMv.changePrecision(MV_PRECISION_INT, MV_PRECISION_INTERNAL);
    pu.mv[REF_PIC_LIST_0] = cMv; // store in fractional pel accuracy

    pu.mvpIdx[REF_PIC_LIST_0] = bvpIdxBest;

    if(pu.cu->imv == 2 && cMv != amvpInfo4Pel.mvCand[bvpIdxBest])
    {
      pu.mvd[REF_PIC_LIST_0] = cMv - amvpInfo4Pel.mvCand[bvpIdxBest];
    }
    else
    {
      pu.mvd[REF_PIC_LIST_0] = cMv - amvpInfo.mvCand[bvpIdxBest];
    }

    if (pu.mvd[REF_PIC_LIST_0] == Mv(0, 0))
    {
      pu.cu->imv = 0;
    }
    if (pu.cu->imv == 2)
    {
      assert((cMv.getHor() % 16 == 0) && (cMv.getVer() % 16 == 0));
    }
    if (cu.cs->sps->getAMVREnabledFlag())
    {
      assert(pu.cu->imv>0 || pu.mvd[REF_PIC_LIST_0] == Mv());
    }

    pu.refIdx[REF_PIC_LIST_0] = MAX_NUM_REF;

  }

  return true;
}

void InterSearch::xxIBCHashSearch(PredictionUnit& pu, Mv* mvPred, int numMvPred, Mv &mv, int& idxMvPred, IbcHashMap& ibcHashMap)
{
  mv.setZero();
  m_pcRdCost->setCostScale(0);

#if GDR_ENABLED
  CodingStructure &cs = *pu.cs;
  const bool isEncodeGdrClean = cs.sps->getGDREnabledFlag() && cs.pcv->isEncoder && ((cs.picHeader->getInGdrInterval() && cs.isClean(pu.Y().topRight(), CHANNEL_TYPE_LUMA)) || (cs.picHeader->getNumVerVirtualBoundaries() == 0));
#endif
  std::vector<Position> candPos;
  if (ibcHashMap.ibcHashMatch(pu.Y(), candPos, *pu.cs, m_pcEncCfg->getIBCHashSearchMaxCand(), m_pcEncCfg->getIBCHashSearchRange4SmallBlk()))
  {
    unsigned int minCost = MAX_UINT;

    const unsigned int  lcuWidth = pu.cs->slice->getSPS()->getMaxCUWidth();
    const int   cuPelX = pu.Y().x;
    const int   cuPelY = pu.Y().y;
    const int   picWidth = pu.cs->slice->getPPS()->getPicWidthInLumaSamples();
    const int   picHeight = pu.cs->slice->getPPS()->getPicHeightInLumaSamples();
    int         roiWidth = pu.lwidth();
    int         roiHeight = pu.lheight();

    for (std::vector<Position>::iterator pos = candPos.begin(); pos != candPos.end(); pos++)
    {
      Position bottomRight = pos->offset(pu.Y().width - 1, pu.Y().height - 1);
      if (pu.cs->isDecomp(*pos, CHANNEL_TYPE_LUMA) && pu.cs->isDecomp(bottomRight, CHANNEL_TYPE_LUMA))
      {
        Position tmp = *pos - pu.Y().pos();
        Mv candMv;
        candMv.set(tmp.x, tmp.y);

        if (!searchBv(pu, cuPelX, cuPelY, roiWidth, roiHeight, picWidth, picHeight, candMv.getHor(), candMv.getVer(), lcuWidth))
        {
          continue;
        }
#if GDR_ENABLED
        Position BvBR(cuPelX + roiWidth + candMv.getHor() - 1, cuPelY + roiHeight + candMv.getVer() - 1);
        if (isEncodeGdrClean && !cs.isClean(BvBR, CHANNEL_TYPE_LUMA))
        {
          continue;
        }
#endif

        for (int n = 0; n < numMvPred; n++)
        {
          m_pcRdCost->setPredictor(mvPred[n]);

          unsigned int cost = m_pcRdCost->getBitsOfVectorWithPredictor(candMv.getHor(), candMv.getVer(), 0);

          if (cost < minCost)
          {
            mv = candMv;
            idxMvPred = n;
            minCost = cost;
          }

          int costQuadPel = MAX_UINT;
          if ((candMv.getHor() % 4 == 0) && (candMv.getVer() % 4 == 0) && (pu.cs->sps->getAMVREnabledFlag()))
          {
            Mv mvPredQuadPel;
            int imvShift = 2;
            int offset = 1 << (imvShift - 1);

            int x = (mvPred[n].hor + offset - (mvPred[n].hor >= 0)) >> 2;
            int y = (mvPred[n].ver + offset - (mvPred[n].ver >= 0)) >> 2;
            mvPredQuadPel.set(x, y);

            m_pcRdCost->setPredictor(mvPredQuadPel);

            costQuadPel = m_pcRdCost->getBitsOfVectorWithPredictor(candMv.getHor() >> 2, candMv.getVer() >> 2, 0);

          }
          if (costQuadPel < minCost)
          {
            mv = candMv;
            idxMvPred = n;
            minCost = costQuadPel;
          }

        }
      }
    }
  }

}


void InterSearch::addToSortList(std::list<BlockHash>& listBlockHash, std::list<int>& listCost, int cost, const BlockHash& blockHash)
{
  std::list<BlockHash>::iterator itBlockHash = listBlockHash.begin();
  std::list<int>::iterator itCost = listCost.begin();

  while (itCost != listCost.end())
  {
    if (cost < (*itCost))
    {
      listCost.insert(itCost, cost);
      listBlockHash.insert(itBlockHash, blockHash);
      return;
    }

    ++itCost;
    ++itBlockHash;
  }

  listCost.push_back(cost);
  listBlockHash.push_back(blockHash);
}

void InterSearch::selectMatchesInter(const MapIterator& itBegin, int count, std::list<BlockHash>& listBlockHash, const BlockHash& currBlockHash)
{
  const int maxReturnNumber = 5;

  listBlockHash.clear();
  std::list<int> listCost;
  listCost.clear();

  MapIterator it = itBegin;
  for (int i = 0; i < count; i++, it++)
  {
    if ((*it).hashValue2 != currBlockHash.hashValue2)
    {
      continue;
    }

    int currCost = RdCost::xGetExpGolombNumberOfBits((*it).x - currBlockHash.x) +
      RdCost::xGetExpGolombNumberOfBits((*it).y - currBlockHash.y);

    if (listBlockHash.size() < maxReturnNumber)
    {
      addToSortList(listBlockHash, listCost, currCost, (*it));
    }
    else if (!listCost.empty() && currCost < listCost.back())
    {
      listCost.pop_back();
      listBlockHash.pop_back();
      addToSortList(listBlockHash, listCost, currCost, (*it));
    }
  }
}
void InterSearch::selectRectangleMatchesInter(const MapIterator& itBegin, int count, std::list<BlockHash>& listBlockHash, const BlockHash& currBlockHash, int width, int height, int idxNonSimple, unsigned int* &hashValues, int baseNum, int picWidth, int picHeight, bool isHorizontal, uint16_t* curHashPic)
{
  const int maxReturnNumber = 5;
  int baseSize = min(width, height);
  unsigned int crcMask = 1 << 16;
  crcMask -= 1;

  listBlockHash.clear();
  std::list<int> listCost;
  listCost.clear();

  MapIterator it = itBegin;

  for (int i = 0; i < count; i++, it++)
  {
    if ((*it).hashValue2 != currBlockHash.hashValue2)
    {
      continue;
    }
    int xRef = (*it).x;
    int yRef = (*it).y;
    if (isHorizontal)
    {
      xRef -= idxNonSimple * baseSize;
    }
    else
    {
      yRef -= idxNonSimple * baseSize;
    }
    if (xRef < 0 || yRef < 0 || xRef + width >= picWidth || yRef + height >= picHeight)
    {
      continue;
    }
    //check Other baseSize hash values
    uint16_t* refHashValue = curHashPic + yRef * picWidth + xRef;
    bool isSame = true;

    for (int k = 0; k < baseNum; k++)
    {
      if ((*refHashValue) != (uint16_t)(hashValues[k] & crcMask))
      {
        isSame = false;
        break;
      }
      refHashValue += (isHorizontal ? baseSize : (baseSize*picWidth));
    }
    if (!isSame)
    {
      continue;
    }

    int currCost = RdCost::xGetExpGolombNumberOfBits(xRef - currBlockHash.x) +
      RdCost::xGetExpGolombNumberOfBits(yRef - currBlockHash.y);

    BlockHash refBlockHash;
    refBlockHash.hashValue2 = (*it).hashValue2;
    refBlockHash.x = xRef;
    refBlockHash.y = yRef;

    if (listBlockHash.size() < maxReturnNumber)
    {
      addToSortList(listBlockHash, listCost, currCost, refBlockHash);
    }
    else if (!listCost.empty() && currCost < listCost.back())
    {
      listCost.pop_back();
      listBlockHash.pop_back();
      addToSortList(listBlockHash, listCost, currCost, refBlockHash);
    }
  }
}

bool InterSearch::xRectHashInterEstimation(PredictionUnit& pu, RefPicList& bestRefPicList, int& bestRefIndex, Mv& bestMv, Mv& bestMvd, int& bestMVPIndex, bool& isPerfectMatch)
{
  int width = pu.cu->lumaSize().width;
  int height = pu.cu->lumaSize().height;

  int baseSize = min(width, height);
  bool isHorizontal = true;;
  int baseNum = 0;
  if (height < width)
  {
    isHorizontal = true;
    baseNum = 1 << (floorLog2(width) - floorLog2(height));
  }
  else
  {
    isHorizontal = false;
    baseNum = 1 << (floorLog2(height) - floorLog2(width));
  }

  int xPos = pu.cu->lumaPos().x;
  int yPos = pu.cu->lumaPos().y;
  const int currStride = pu.cs->picture->getOrigBuf().get(COMPONENT_Y).stride;
  const Pel* curPel = pu.cs->picture->getOrigBuf().get(COMPONENT_Y).buf + yPos * currStride + xPos;
  int picWidth = pu.cu->slice->getPPS()->getPicWidthInLumaSamples();
  int picHeight = pu.cu->slice->getPPS()->getPicHeightInLumaSamples();

  int xBase = xPos;
  int yBase = yPos;
  const Pel* basePel = curPel;
  int idxNonSimple = -1;
  unsigned int* hashValue1s = new unsigned int[baseNum];
  unsigned int* hashValue2s = new unsigned int[baseNum];

#if GDR_ENABLED
  CodingStructure &cs = *pu.cs;
  const bool isEncodeGdrClean = cs.sps->getGDREnabledFlag() && cs.pcv->isEncoder && ((cs.picHeader->getInGdrInterval() && cs.isClean(pu.Y().topRight(), CHANNEL_TYPE_LUMA)) || (cs.picHeader->getNumVerVirtualBoundaries() == 0));
#endif

  for (int k = 0; k < baseNum; k++)
  {
    if (isHorizontal)
    {
      xBase = xPos + k * baseSize;
      basePel = curPel + k * baseSize;
    }
    else
    {
      yBase = yPos + k * baseSize;
      basePel = curPel + k * baseSize * currStride;
    }

    if (idxNonSimple == -1 && !TComHash::isHorizontalPerfectLuma(basePel, currStride, baseSize, baseSize) && !TComHash::isVerticalPerfectLuma(basePel, currStride, baseSize, baseSize))
    {
      idxNonSimple = k;
    }
    TComHash::getBlockHashValue((pu.cs->picture->getOrigBuf()), baseSize, baseSize, xBase, yBase, pu.cu->slice->getSPS()->getBitDepths(), hashValue1s[k], hashValue2s[k]);
  }
  if (idxNonSimple == -1)
  {
    idxNonSimple = 0;
  }

  Distortion bestCost = UINT64_MAX;

  BlockHash currBlockHash;
  currBlockHash.x = xPos;//still use the first base block location
  currBlockHash.y = yPos;

  currBlockHash.hashValue2 = hashValue2s[idxNonSimple];

  m_pcRdCost->setDistParam(m_cDistParam, pu.cs->getOrgBuf(pu).Y(), 0, 0, m_lumaClpRng.bd, COMPONENT_Y, 0, 1, false);

  int imvBest = 0;
  int numPredDir = pu.cu->slice->isInterP() ? 1 : 2;
  for (int refList = 0; refList < numPredDir; refList++)
  {
    RefPicList eRefPicList = (refList == 0) ? REF_PIC_LIST_0 : REF_PIC_LIST_1;
    int refPicNumber = pu.cu->slice->getNumRefIdx(eRefPicList);

    for (int refIdx = 0; refIdx < refPicNumber; refIdx++)
    {
      int bitsOnRefIdx = 1;
      if (refPicNumber > 1)
      {
        bitsOnRefIdx += refIdx + 1;
        if (refIdx == refPicNumber - 1)
        {
          bitsOnRefIdx--;
        }
      }
      m_numHashMVStoreds[eRefPicList][refIdx] = 0;

      const std::pair<int, int>& scaleRatio = pu.cu->slice->getScalingRatio( eRefPicList, refIdx );
      if( scaleRatio != SCALE_1X )
      {
        continue;
      }

      CHECK( pu.cu->slice->getRefPic( eRefPicList, refIdx )->getHashMap() == nullptr, "Hash table is not initialized" );

      if (refList == 0 || pu.cu->slice->getList1IdxToList0Idx(refIdx) < 0)
      {
        int count = static_cast<int>(pu.cu->slice->getRefPic(eRefPicList, refIdx)->getHashMap()->count(hashValue1s[idxNonSimple]));
        if (count == 0)
        {
          continue;
        }

        list<BlockHash> listBlockHash;
        selectRectangleMatchesInter(pu.cu->slice->getRefPic(eRefPicList, refIdx)->getHashMap()->getFirstIterator(hashValue1s[idxNonSimple]), count, listBlockHash, currBlockHash, width, height, idxNonSimple, hashValue2s, baseNum, picWidth, picHeight, isHorizontal, pu.cu->slice->getRefPic(eRefPicList, refIdx)->getHashMap()->getHashPic(baseSize));

        m_numHashMVStoreds[eRefPicList][refIdx] = int(listBlockHash.size());
        if (listBlockHash.empty())
        {
          continue;
        }
        AMVPInfo currAMVPInfoPel;
        AMVPInfo currAMVPInfo4Pel;
        AMVPInfo currAMVPInfoQPel;

#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          currAMVPInfoPel.allCandSolidInAbove = true;
          currAMVPInfo4Pel.allCandSolidInAbove = true;
          currAMVPInfoQPel.allCandSolidInAbove = true;

          for (int i = 0; i < AMVP_MAX_NUM_CANDS_MEM; i++)
          {
            currAMVPInfoPel.mvSolid[i] = true;
            currAMVPInfoPel.mvValid[i] = true;
            currAMVPInfo4Pel.mvSolid[i] = true;
            currAMVPInfo4Pel.mvValid[i] = true;
            currAMVPInfoQPel.mvSolid[i] = true;
            currAMVPInfoQPel.mvValid[i] = true;
          }
        }
#endif

        pu.cu->imv = 2;
        PU::fillMvpCand(pu, eRefPicList, refIdx, currAMVPInfo4Pel);
        pu.cu->imv = 1;
        PU::fillMvpCand(pu, eRefPicList, refIdx, currAMVPInfoPel);
        pu.cu->imv = 0;
        PU::fillMvpCand(pu, eRefPicList, refIdx, currAMVPInfoQPel);
        for (int mvpIdxTemp = 0; mvpIdxTemp < 2; mvpIdxTemp++)
        {
          currAMVPInfoQPel.mvCand[mvpIdxTemp].changePrecision(MV_PRECISION_INTERNAL, MV_PRECISION_QUARTER);
          currAMVPInfoPel.mvCand[mvpIdxTemp].changePrecision(MV_PRECISION_INTERNAL, MV_PRECISION_QUARTER);
          currAMVPInfo4Pel.mvCand[mvpIdxTemp].changePrecision(MV_PRECISION_INTERNAL, MV_PRECISION_QUARTER);
        }

        bool wrap = pu.cu->slice->getRefPic(eRefPicList, refIdx)->isWrapAroundEnabled( pu.cs->pps );
        const Pel* refBufStart = pu.cu->slice->getRefPic(eRefPicList, refIdx)->getRecoBuf(wrap).get(COMPONENT_Y).buf;
        const int refStride = pu.cu->slice->getRefPic(eRefPicList, refIdx)->getRecoBuf(wrap).get(COMPONENT_Y).stride;
        m_cDistParam.cur.stride = refStride;

        m_pcRdCost->selectMotionLambda( );
        m_pcRdCost->setCostScale(0);

        list<BlockHash>::iterator it;
        int countMV = 0;
        for (it = listBlockHash.begin(); it != listBlockHash.end(); ++it)
        {
          int curMVPIdx = 0;
          unsigned int curMVPbits = MAX_UINT;
          Mv cMv((*it).x - currBlockHash.x, (*it).y - currBlockHash.y);
          m_hashMVStoreds[eRefPicList][refIdx][countMV++] = cMv;
          cMv.changePrecision(MV_PRECISION_INT, MV_PRECISION_QUARTER);

#if GDR_ENABLED
          bool allOk = true;
          bool anyCandOk = false;
          bool Valid = true;
          if (isEncodeGdrClean)
          {
            Mv cMv16 = cMv;
            cMv16.changePrecision(MV_PRECISION_QUARTER, MV_PRECISION_INTERNAL);
            const Position bottomRight = pu.Y().bottomRight();
            Valid = cs.isClean(bottomRight, cMv16, eRefPicList, refIdx);
          }
#endif

#if GDR_ENABLED
          if (!Valid)
          {
            continue;
          }
#endif
          for (int mvpIdxTemp = 0; mvpIdxTemp < 2; mvpIdxTemp++)
          {
            Mv cMvPredPel = currAMVPInfoQPel.mvCand[mvpIdxTemp];
            m_pcRdCost->setPredictor(cMvPredPel);

            unsigned int tempMVPbits = m_pcRdCost->getBitsOfVectorWithPredictor(cMv.getHor(), cMv.getVer(), 0);

#if GDR_ENABLED
            allOk = (tempMVPbits < curMVPbits);
            if (isEncodeGdrClean)
            {
              bool isSolid = currAMVPInfoQPel.mvSolid[mvpIdxTemp];
              allOk = allOk && isSolid;
              if (allOk) anyCandOk = true;
            }
#endif

#if GDR_ENABLED
            if (allOk)
#else
            if (tempMVPbits < curMVPbits)
#endif
            {
              curMVPbits = tempMVPbits;
              curMVPIdx = mvpIdxTemp;
              pu.cu->imv = 0;
            }

            if (pu.cu->slice->getSPS()->getAMVREnabledFlag())
            {
              unsigned int bitsMVP1Pel = MAX_UINT;
              Mv mvPred1Pel = currAMVPInfoPel.mvCand[mvpIdxTemp];
              m_pcRdCost->setPredictor(mvPred1Pel);
              bitsMVP1Pel = m_pcRdCost->getBitsOfVectorWithPredictor(cMv.getHor(), cMv.getVer(), 2);
#if GDR_ENABLED
              allOk = (bitsMVP1Pel < curMVPbits);
              if (isEncodeGdrClean)
              {
                bool isSolid = currAMVPInfoPel.mvSolid[mvpIdxTemp];
                allOk = allOk && isSolid;
                if (allOk)
                {
                  anyCandOk = true;
                }
              }
#endif

#if GDR_ENABLED
              if (allOk)
#else
              if (bitsMVP1Pel < curMVPbits)
#endif
              {
                curMVPbits = bitsMVP1Pel;
                curMVPIdx = mvpIdxTemp;
                pu.cu->imv = 1;
              }

              if ((cMv.getHor() % 16 == 0) && (cMv.getVer() % 16 == 0))
              {
                unsigned int bitsMVP4Pel = MAX_UINT;
                Mv mvPred4Pel = currAMVPInfo4Pel.mvCand[mvpIdxTemp];
                m_pcRdCost->setPredictor(mvPred4Pel);
                bitsMVP4Pel = m_pcRdCost->getBitsOfVectorWithPredictor(cMv.getHor(), cMv.getVer(), 4);
#if GDR_ENABLED
                allOk = (bitsMVP4Pel < curMVPbits);
                if (isEncodeGdrClean)
                {
                  bool isSolid = currAMVPInfo4Pel.mvSolid[mvpIdxTemp];
                  allOk = allOk && isSolid;
                  if (allOk)
                  {
                    anyCandOk = true;
                  }
                }
#endif

#if GDR_ENABLED
                if (allOk)
#else
                if (bitsMVP4Pel < curMVPbits)
#endif
                {
                  curMVPbits = bitsMVP4Pel;
                  curMVPIdx = mvpIdxTemp;
                  pu.cu->imv = 2;
                }
              }
            }
          }

#if GDR_ENABLED
          if (isEncodeGdrClean && !anyCandOk)
          {
            continue;
          }
#endif

          curMVPbits += bitsOnRefIdx;

          m_cDistParam.cur.buf = refBufStart + (*it).y*refStride + (*it).x;
          Distortion currSad = m_cDistParam.distFunc(m_cDistParam);
          Distortion currCost = currSad + m_pcRdCost->getCost(curMVPbits);

          if (!isPerfectMatch)
          {
            if (pu.cu->slice->getRefPic(eRefPicList, refIdx)->slices[0]->getSliceQp() <= pu.cu->slice->getSliceQp())
            {
              isPerfectMatch = true;
            }
          }

          if (currCost < bestCost)
          {
            bestCost = currCost;
            bestRefPicList = eRefPicList;
            bestRefIndex = refIdx;
            bestMv = cMv;
            bestMVPIndex = curMVPIdx;
            imvBest = pu.cu->imv;
            if (pu.cu->imv == 2)
            {
              bestMvd = cMv - currAMVPInfo4Pel.mvCand[curMVPIdx];
            }
            else if (pu.cu->imv == 1)
            {
              bestMvd = cMv - currAMVPInfoPel.mvCand[curMVPIdx];
            }
            else
            {
              bestMvd = cMv - currAMVPInfoQPel.mvCand[curMVPIdx];
            }
          }
        }
      }
    }
  }
  delete[] hashValue1s;
  delete[] hashValue2s;
  pu.cu->imv = imvBest;
  if (bestMvd == Mv(0, 0))
  {
    pu.cu->imv = 0;
    return false;
  }
  return (bestCost < MAX_INT);
}

bool InterSearch::xHashInterEstimation(PredictionUnit& pu, RefPicList& bestRefPicList, int& bestRefIndex, Mv& bestMv, Mv& bestMvd, int& bestMVPIndex, bool& isPerfectMatch)
{
  int width = pu.cu->lumaSize().width;
  int height = pu.cu->lumaSize().height;
  if (width != height)
  {
    return xRectHashInterEstimation(pu, bestRefPicList, bestRefIndex, bestMv, bestMvd, bestMVPIndex, isPerfectMatch);
  }
  int xPos = pu.cu->lumaPos().x;
  int yPos = pu.cu->lumaPos().y;

  uint32_t hashValue1;
  uint32_t hashValue2;
  Distortion bestCost = UINT64_MAX;

  if (!TComHash::getBlockHashValue((pu.cs->picture->getOrigBuf()), width, height, xPos, yPos, pu.cu->slice->getSPS()->getBitDepths(), hashValue1, hashValue2))
  {
    return false;
  }

#if GDR_ENABLED
  CodingStructure &cs = *pu.cs;
  const bool isEncodeGdrClean = cs.sps->getGDREnabledFlag() && cs.pcv->isEncoder && ((cs.picHeader->getInGdrInterval() && cs.isClean(pu.Y().topRight(), CHANNEL_TYPE_LUMA)) || (cs.picHeader->getNumVerVirtualBoundaries() == 0));
#endif
  BlockHash currBlockHash;
  currBlockHash.x = xPos;
  currBlockHash.y = yPos;
  currBlockHash.hashValue2 = hashValue2;

  m_pcRdCost->setDistParam(m_cDistParam, pu.cs->getOrgBuf(pu).Y(), 0, 0, m_lumaClpRng.bd, COMPONENT_Y, 0, 1, false);

  int imvBest = 0;

  int numPredDir = pu.cu->slice->isInterP() ? 1 : 2;
  for (int refList = 0; refList < numPredDir; refList++)
  {
    RefPicList eRefPicList = (refList == 0) ? REF_PIC_LIST_0 : REF_PIC_LIST_1;
    int refPicNumber = pu.cu->slice->getNumRefIdx(eRefPicList);


    for (int refIdx = 0; refIdx < refPicNumber; refIdx++)
    {
      int bitsOnRefIdx = 1;
      if (refPicNumber > 1)
      {
        bitsOnRefIdx += refIdx + 1;
        if (refIdx == refPicNumber - 1)
        {
          bitsOnRefIdx--;
        }
      }
      m_numHashMVStoreds[eRefPicList][refIdx] = 0;

      const std::pair<int, int>& scaleRatio = pu.cu->slice->getScalingRatio( eRefPicList, refIdx );
      if( scaleRatio != SCALE_1X )
      {
        continue;
      }

      CHECK( pu.cu->slice->getRefPic( eRefPicList, refIdx )->getHashMap() == nullptr, "Hash table is not initialized" );

      if (refList == 0 || pu.cu->slice->getList1IdxToList0Idx(refIdx) < 0)
      {
        int count = static_cast<int>(pu.cu->slice->getRefPic(eRefPicList, refIdx)->getHashMap()->count(hashValue1));
        if (count == 0)
        {
          continue;
        }

        list<BlockHash> listBlockHash;
        selectMatchesInter(pu.cu->slice->getRefPic(eRefPicList, refIdx)->getHashMap()->getFirstIterator(hashValue1), count, listBlockHash, currBlockHash);
        m_numHashMVStoreds[eRefPicList][refIdx] = (int)listBlockHash.size();
        if (listBlockHash.empty())
        {
          continue;
        }
        AMVPInfo currAMVPInfoPel;
        AMVPInfo currAMVPInfo4Pel;
#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          currAMVPInfo4Pel.allCandSolidInAbove = true;
          for (int i = 0; i < AMVP_MAX_NUM_CANDS_MEM; i++)
          {
            currAMVPInfo4Pel.mvSolid[i] = true;
            currAMVPInfo4Pel.mvValid[i] = true;
          }
        }
#endif
        pu.cu->imv = 2;
        PU::fillMvpCand(pu, eRefPicList, refIdx, currAMVPInfo4Pel);

#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          currAMVPInfoPel.allCandSolidInAbove = true;
          for (int i = 0; i < AMVP_MAX_NUM_CANDS_MEM; i++)
          {
            currAMVPInfoPel.mvSolid[i] = true;
            currAMVPInfoPel.mvValid[i] = true;
          }
        }
#endif
        pu.cu->imv = 1;
        PU::fillMvpCand(pu, eRefPicList, refIdx, currAMVPInfoPel);
        AMVPInfo currAMVPInfoQPel;
#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          currAMVPInfoQPel.allCandSolidInAbove = true;
          for (int i = 0; i < AMVP_MAX_NUM_CANDS_MEM; i++)
          {
            currAMVPInfoQPel.mvSolid[i] = true;
            currAMVPInfoQPel.mvValid[i] = true;
          }
        }
#endif
        pu.cu->imv = 0;
        PU::fillMvpCand(pu, eRefPicList, refIdx, currAMVPInfoQPel);
        CHECK(currAMVPInfoPel.numCand <= 1, "Wrong")
        for (int mvpIdxTemp = 0; mvpIdxTemp < 2; mvpIdxTemp++)
        {
          currAMVPInfoQPel.mvCand[mvpIdxTemp].changePrecision(MV_PRECISION_INTERNAL, MV_PRECISION_QUARTER);
          currAMVPInfoPel.mvCand[mvpIdxTemp].changePrecision(MV_PRECISION_INTERNAL, MV_PRECISION_QUARTER);
          currAMVPInfo4Pel.mvCand[mvpIdxTemp].changePrecision(MV_PRECISION_INTERNAL, MV_PRECISION_QUARTER);
        }

        bool wrap = pu.cu->slice->getRefPic(eRefPicList, refIdx)->isWrapAroundEnabled( pu.cs->pps );
        const Pel* refBufStart = pu.cu->slice->getRefPic(eRefPicList, refIdx)->getRecoBuf(wrap).get(COMPONENT_Y).buf;
        const int refStride = pu.cu->slice->getRefPic(eRefPicList, refIdx)->getRecoBuf(wrap).get(COMPONENT_Y).stride;

        m_cDistParam.cur.stride = refStride;

        m_pcRdCost->selectMotionLambda( );
        m_pcRdCost->setCostScale(0);

        list<BlockHash>::iterator it;
        int countMV = 0;
        for (it = listBlockHash.begin(); it != listBlockHash.end(); ++it)
        {
          int curMVPIdx = 0;
          unsigned int curMVPbits = MAX_UINT;
          Mv cMv((*it).x - currBlockHash.x, (*it).y - currBlockHash.y);
          m_hashMVStoreds[eRefPicList][refIdx][countMV++] = cMv;
          cMv.changePrecision(MV_PRECISION_INT, MV_PRECISION_QUARTER);

#if GDR_ENABLED
          bool Valid = true;
          bool allOk = true;
          bool anyCandOk = false;

          if (isEncodeGdrClean)
          {
            Mv cMv16 = cMv;
            cMv16.changePrecision(MV_PRECISION_QUARTER, MV_PRECISION_INTERNAL);
            const Position bottomRight = pu.Y().bottomRight();
            Valid = cs.isClean(bottomRight, cMv16, eRefPicList, refIdx);
          }
#endif

#if GDR_ENABLED
          if (!Valid)
          {
            continue;
          }
#endif

          for (int mvpIdxTemp = 0; mvpIdxTemp < 2; mvpIdxTemp++)
          {
            Mv cMvPredPel = currAMVPInfoQPel.mvCand[mvpIdxTemp];
            m_pcRdCost->setPredictor(cMvPredPel);

            unsigned int tempMVPbits = m_pcRdCost->getBitsOfVectorWithPredictor(cMv.getHor(), cMv.getVer(), 0);

#if GDR_ENABLED
            allOk = (tempMVPbits < curMVPbits);
            if (isEncodeGdrClean)
            {
              bool isSolid = currAMVPInfoQPel.mvSolid[mvpIdxTemp];
              allOk = allOk && isSolid;
              if (allOk) anyCandOk = true;
            }
#endif

#if GDR_ENABLED
            if (allOk)
#else
            if (tempMVPbits < curMVPbits)
#endif
            {
              curMVPbits = tempMVPbits;
              curMVPIdx = mvpIdxTemp;
              pu.cu->imv = 0;
            }

            if (pu.cu->slice->getSPS()->getAMVREnabledFlag())
            {
              unsigned int bitsMVP1Pel = MAX_UINT;
              Mv mvPred1Pel = currAMVPInfoPel.mvCand[mvpIdxTemp];
              m_pcRdCost->setPredictor(mvPred1Pel);
              bitsMVP1Pel = m_pcRdCost->getBitsOfVectorWithPredictor(cMv.getHor(), cMv.getVer(), 2);
#if GDR_ENABLED
              allOk = (bitsMVP1Pel < curMVPbits);
              if (isEncodeGdrClean)
              {
                bool isSolid = currAMVPInfoPel.mvSolid[mvpIdxTemp];
                allOk = allOk && isSolid;
                if (allOk) anyCandOk = true;
              }
#endif

#if GDR_ENABLED
              if (allOk)
#else
              if (bitsMVP1Pel < curMVPbits)
#endif
              {
                curMVPbits = bitsMVP1Pel;
                curMVPIdx = mvpIdxTemp;
                pu.cu->imv = 1;
              }

              if ((cMv.getHor() % 16 == 0) && (cMv.getVer() % 16 == 0))
              {
                unsigned int bitsMVP4Pel = MAX_UINT;
                Mv mvPred4Pel = currAMVPInfo4Pel.mvCand[mvpIdxTemp];
                m_pcRdCost->setPredictor(mvPred4Pel);
                bitsMVP4Pel = m_pcRdCost->getBitsOfVectorWithPredictor(cMv.getHor(), cMv.getVer(), 4);

#if GDR_ENABLED
                allOk = (bitsMVP4Pel < curMVPbits);
                if (isEncodeGdrClean)
                {
                  bool isSolid = currAMVPInfo4Pel.mvSolid[mvpIdxTemp];
                  allOk = allOk && isSolid;
                  if (allOk) anyCandOk = true;
                }
#endif

#if GDR_ENABLED
                if (allOk)
#else
                if (bitsMVP4Pel < curMVPbits)
#endif
                {
                  curMVPbits = bitsMVP4Pel;
                  curMVPIdx = mvpIdxTemp;
                  pu.cu->imv = 2;
                }
              }
            }
          }

#if GDR_ENABLED
          if (isEncodeGdrClean && !anyCandOk)
          {
            continue;
          }
#endif

          curMVPbits += bitsOnRefIdx;

          m_cDistParam.cur.buf = refBufStart + (*it).y*refStride + (*it).x;
          Distortion currSad = m_cDistParam.distFunc(m_cDistParam);
          Distortion currCost = currSad + m_pcRdCost->getCost(curMVPbits);

          if (!isPerfectMatch)
          {
            if (pu.cu->slice->getRefPic(eRefPicList, refIdx)->slices[0]->getSliceQp() <= pu.cu->slice->getSliceQp())
            {
              isPerfectMatch = true;
            }
          }

          if (currCost < bestCost)
          {
            bestCost = currCost;
            bestRefPicList = eRefPicList;
            bestRefIndex = refIdx;
            bestMv = cMv;
            bestMVPIndex = curMVPIdx;
            imvBest = pu.cu->imv;
            if (pu.cu->imv == 2)
            {
              bestMvd = cMv - currAMVPInfo4Pel.mvCand[curMVPIdx];
            }
            else if (pu.cu->imv == 1)
            {
              bestMvd = cMv - currAMVPInfoPel.mvCand[curMVPIdx];
            }
            else
            {
              bestMvd = cMv - currAMVPInfoQPel.mvCand[curMVPIdx];
            }
          }
        }
      }
    }
  }
  pu.cu->imv = imvBest;
  if (bestMvd == Mv(0, 0))
  {
    pu.cu->imv = 0;
    return false;
  }
  return (bestCost < MAX_INT);
}

bool InterSearch::predInterHashSearch(CodingUnit& cu, Partitioner& partitioner, bool& isPerfectMatch)
{
  Mv       bestMv, bestMvd;
  RefPicList   bestRefPicList;
  int          bestRefIndex;
  int          bestMVPIndex;

  auto &pu = *cu.firstPU;

  Mv cMvZero;
  pu.mv[REF_PIC_LIST_0] = Mv();
  pu.mv[REF_PIC_LIST_1] = Mv();
  pu.mvd[REF_PIC_LIST_0] = cMvZero;
  pu.mvd[REF_PIC_LIST_1] = cMvZero;
  pu.refIdx[REF_PIC_LIST_0] = NOT_VALID;
  pu.refIdx[REF_PIC_LIST_1] = NOT_VALID;
  pu.mvpIdx[REF_PIC_LIST_0] = NOT_VALID;
  pu.mvpIdx[REF_PIC_LIST_1] = NOT_VALID;
  pu.mvpNum[REF_PIC_LIST_0] = NOT_VALID;
  pu.mvpNum[REF_PIC_LIST_1] = NOT_VALID;

  if (xHashInterEstimation(pu, bestRefPicList, bestRefIndex, bestMv, bestMvd, bestMVPIndex, isPerfectMatch))
  {
    pu.interDir = static_cast<int>(bestRefPicList) + 1;
    pu.mv[bestRefPicList] = bestMv;
    pu.mv[bestRefPicList].changePrecision(MV_PRECISION_QUARTER, MV_PRECISION_INTERNAL);

    pu.mvd[bestRefPicList] = bestMvd;
    pu.mvd[bestRefPicList].changePrecision(MV_PRECISION_QUARTER, MV_PRECISION_INTERNAL);
    pu.refIdx[bestRefPicList] = bestRefIndex;
    pu.mvpIdx[bestRefPicList] = bestMVPIndex;

    pu.mvpNum[bestRefPicList] = 2;

    PU::spanMotionInfo(pu);
    PelUnitBuf predBuf = pu.cs->getPredBuf(pu);
    motionCompensation(pu, predBuf, REF_PIC_LIST_X);
    return true;
  }
  else
  {
    return false;
  }

  return true;
}


//! search of the best candidate for inter prediction
void InterSearch::predInterSearch(CodingUnit& cu, Partitioner& partitioner)
{
  CodingStructure& cs = *cu.cs;

  AMVPInfo     amvp[2];
  Mv           cMvSrchRngLT;
  Mv           cMvSrchRngRB;

  Mv           cMvZero;

  Mv           cMv[2];
  Mv           cMvBi[2];
  Mv           cMvTemp[2][33];
  Mv           cMvHevcTemp[2][33];
  int          iNumPredDir = cs.slice->isInterP() ? 1 : 2;

  Mv           cMvPred[2][33];

  Mv           cMvPredBi[2][33];
  int          aaiMvpIdxBi[2][33];

  int          aaiMvpIdx[2][33];
  int          aaiMvpNum[2][33];

#if GDR_ENABLED
  bool         cMvSolid[2];
  bool         cMvValid[2];
  bool         cMvBiSolid[2];
  bool         cMvBiValid[2];

  bool         cMvPredSolid[2][33];
  bool         cMvPredBiSolid[2][33];

  bool         cMvTempSolid[2][33]{ { true } };
  bool         cMvTempValid[2][33];

  bool         cMvHevcTempSolid[2][33];
  bool         cMvHevcTempValid[2][33];

  bool         allOk;
  bool         bestBiPDistOk;
  bool         biPDistTempOk;
  bool         uiCostTempOk;
  bool         uiCostTempL0Ok[MAX_NUM_REF];

  bool         uiHevcCostOk;
  bool         uiAffineCostOk;
  bool         uiAffine6CostOk;
  bool         uiCostOk[2];
  bool         uiCostBiOk;
  bool         costValidList1Ok;

  bool         bCleanCandExist;
#endif

  AMVPInfo     aacAMVPInfo[2][33];

  int refIdx[2] = { 0, 0 };   // If un-initialized, may cause SEGV in bi-directional prediction iterative stage.
  int          iRefIdxBi[2] = { -1, -1 };

  uint32_t mbBits[3] = { 1, 1, 0 };

  uint32_t         uiLastMode = 0;
  uint32_t         uiLastModeTemp = 0;
  int          iRefStart, iRefEnd;

  int          symMode = 0;

  int          bestBiPRefIdxL1 = 0;
  int          bestBiPMvpL1    = 0;
  Distortion   biPDistTemp     = std::numeric_limits<Distortion>::max();

  uint8_t      bcwIdx          = (cu.cs->slice->isInterB() ? cu.BcwIdx : BCW_DEFAULT);
  bool         enforceBcwPred = false;
  MergeCtx     mergeCtx;

  // Loop over Prediction Units
  CHECK(!cu.firstPU, "CU does not contain any PUs");
  uint32_t         puIdx = 0;
  auto &pu = *cu.firstPU;
  WPScalingParam *wp0;
  WPScalingParam *wp1;
  int tryBipred = 0;
  bool checkAffine    = (pu.cu->imv == 0 || pu.cu->slice->getSPS()->getAffineAmvrEnabledFlag()) && pu.cu->imv != IMV_HPEL;
  bool checkNonAffine = pu.cu->imv == 0 || pu.cu->imv == IMV_HPEL || (pu.cu->slice->getSPS()->getAMVREnabledFlag() &&
                                            pu.cu->imv <= (pu.cu->slice->getSPS()->getAMVREnabledFlag() ? IMV_4PEL : 0));
  CodingUnit *bestCU  = pu.cu->cs->bestCS != nullptr ? pu.cu->cs->bestCS->getCU( CHANNEL_TYPE_LUMA ) : nullptr;
  bool trySmvd        = ( bestCU != nullptr && pu.cu->imv == 2 && checkAffine ) ? ( !bestCU->firstPU->mergeFlag && !bestCU->affine ) : true;
  if ( pu.cu->imv && bestCU != nullptr && checkAffine )
  {
    checkAffine = !( bestCU->firstPU->mergeFlag || !bestCU->affine );
  }

  if ( pu.cu->imv == 2 && checkNonAffine && pu.cu->slice->getSPS()->getAffineAmvrEnabledFlag() )
  {
    checkNonAffine = m_affineMotion.hevcCost[1] < m_affineMotion.hevcCost[0] * 1.06f;
  }

#if GDR_ENABLED
  const bool isEncodeGdrClean = cs.sps->getGDREnabledFlag() && cs.pcv->isEncoder && ((cs.picHeader->getInGdrInterval() && cs.isClean(pu.Y().topRight(), CHANNEL_TYPE_LUMA)) || (cs.picHeader->getNumVerVirtualBoundaries() == 0));
  const bool init_value = true;
#endif

  amvp[0].numCand = 0;
  amvp[1].numCand = 0;
  memset(aacAMVPInfo, 0, sizeof(aacAMVPInfo));

#if GDR_ENABLED
  if (isEncodeGdrClean)
  {
    biPDistTempOk = init_value;
    bestBiPDistOk = init_value;
    uiCostTempOk = init_value;

    uiHevcCostOk = init_value;
    uiAffineCostOk = init_value;
    uiAffine6CostOk = init_value;
    memset(uiCostOk, init_value, sizeof(uiCostOk));
    uiCostBiOk = init_value;
    uiCostTempOk = init_value;
    costValidList1Ok = init_value;

    memset(cMvSolid, init_value, sizeof(cMvSolid));
    memset(cMvValid, init_value, sizeof(cMvValid));
    memset(cMvBiSolid, !init_value, sizeof(cMvBiSolid));
    memset(cMvBiValid, !init_value, sizeof(cMvBiValid));

    memset(cMvPredSolid, init_value, sizeof(cMvPredSolid));
    memset(cMvPredBiSolid, init_value, sizeof(cMvPredBiSolid));

    memset(cMvTempSolid, init_value, sizeof(cMvTempSolid));
    memset(cMvTempValid, init_value, sizeof(cMvTempValid));
    memset(cMvHevcTempSolid, init_value, sizeof(cMvHevcTempSolid));
    memset(cMvHevcTempValid, init_value, sizeof(cMvHevcTempValid));


    memset(pu.mvSolid, init_value, sizeof(pu.mvSolid));
    memset(pu.mvValid, init_value, sizeof(pu.mvValid));

    memset(pu.mvAffiSolid, init_value, sizeof(pu.mvAffiSolid));
    memset(pu.mvAffiValid, init_value, sizeof(pu.mvAffiValid));

    memset(pu.mvpSolid, init_value, sizeof(pu.mvpSolid));
    memset(pu.mvpType, init_value, sizeof(pu.mvpType));

    pu.mvpPos[0] = Position(0, 0);
    pu.mvpPos[1] = Position(0, 0);

    bCleanCandExist = false;
  }
#endif

  {
    if (pu.cu->cs->bestParent != nullptr && pu.cu->cs->bestParent->getCU(CHANNEL_TYPE_LUMA) != nullptr && pu.cu->cs->bestParent->getCU(CHANNEL_TYPE_LUMA)->affine == false)
    {
      m_skipPROF = true;
    }
    m_encOnly = true;
    // motion estimation only evaluates luma component
    m_maxCompIDToPred = MAX_NUM_COMPONENT;
//    m_maxCompIDToPred = COMPONENT_Y;

    CHECK(pu.cu != &cu, "PU is contained in another CU");

    if (cu.cs->sps->getSbTMVPEnabledFlag())
    {
      Size bufSize = g_miScaling.scale(pu.lumaSize());
      mergeCtx.subPuMvpMiBuf = MotionBuf(m_SubPuMiBuf, bufSize);
    }

    PU::spanMotionInfo( pu );
    Distortion   uiHevcCost = std::numeric_limits<Distortion>::max();
    Distortion   uiAffineCost = std::numeric_limits<Distortion>::max();
    Distortion   uiCost[2] = { std::numeric_limits<Distortion>::max(), std::numeric_limits<Distortion>::max() };
    Distortion   uiCostBi  =   std::numeric_limits<Distortion>::max();
    Distortion   costTemp;

#if GDR_ENABLED
    memset(uiCostTempL0Ok, init_value, sizeof(uiCostTempL0Ok));

    bool mvValidList1Solid = init_value;
    bool mvValidList1Valid = init_value;
    uiHevcCostOk = false;
    uiAffineCostOk = false;
#endif

    uint32_t         bits[3];
    uint32_t         bitsTemp;
    Distortion   bestBiPDist = std::numeric_limits<Distortion>::max();

    Distortion   uiCostTempL0[MAX_NUM_REF];
    for (int iNumRef=0; iNumRef < MAX_NUM_REF; iNumRef++)
    {
      uiCostTempL0[iNumRef] = std::numeric_limits<Distortion>::max();
    }
    uint32_t         uiBitsTempL0[MAX_NUM_REF];

    Mv           mvValidList1;
    int          refIdxValidList1 = 0;
    uint32_t         bitsValidList1   = MAX_UINT;
    Distortion   costValidList1   = std::numeric_limits<Distortion>::max();

    PelUnitBuf origBuf = pu.cs->getOrgBuf( pu );

    xGetBlkBits(cs.slice->isInterP(), puIdx, uiLastMode, mbBits);

    m_pcRdCost->selectMotionLambda( );

    unsigned imvShift = pu.cu->imv == IMV_HPEL ? 1 : (pu.cu->imv << 1);

    //RAMIRO: função para calcular e extrair features
    #if EXTRA_FEATURES
        std::string strIdBloco =  std::to_string(cu.cs->slice->getPOC()) + std::to_string(cu.Y().x) + std::to_string(cu.Y().y)+
                            to_string(cu.Y().width) + to_string(cu.Y().height);

        idBlocoAtual = std::stoul(strIdBloco,nullptr,10);
        blocosIguais = (idBlocoTemp == idBlocoAtual);

        #if EXTRA_DADOS_MOD_1

            if(!blocosIguais && blocoAffAvaliavel){
                extracaoDados::registraFeatures();
                idBlocoTemp = idBlocoAtual;
                buscaPaiVizinho = true;
            }

            extracaoDados::setModoSplit_temp();

        #endif

        #if EXTRA_DADOS_MOD_2
            if(!blocosIguais){
                idBlocoTemp = idBlocoAtual;
                buscaPaiVizinho = true;
            }

            extracaoDados::setModoSplit_temp();

        #endif    

        blocoAffAvaliavel = (cu.Y().width > 8 && cu.Y().height > 8); // verifica se a largura e altura do bloco é maior que 8 
    
    #endif

    if ( checkNonAffine )
    {
      //  Uni-directional prediction
      for (int refList = 0; refList < iNumPredDir; refList++)
      {
        RefPicList eRefPicList = (refList ? REF_PIC_LIST_1 : REF_PIC_LIST_0);
        for (int refIdxTemp = 0; refIdxTemp < cs.slice->getNumRefIdx(eRefPicList); refIdxTemp++)
        {
          bitsTemp = mbBits[refList];
          if ( cs.slice->getNumRefIdx(eRefPicList) > 1 )
          {
            bitsTemp += refIdxTemp + 1;
            if (refIdxTemp == cs.slice->getNumRefIdx(eRefPicList) - 1)
            {
              bitsTemp--;
            }
          }
          xEstimateMvPredAMVP(pu, origBuf, eRefPicList, refIdxTemp, cMvPred[refList][refIdxTemp], amvp[eRefPicList],
                              false, &biPDistTemp);

          aaiMvpIdx[refList][refIdxTemp] = pu.mvpIdx[eRefPicList];
          aaiMvpNum[refList][refIdxTemp] = pu.mvpNum[eRefPicList];
#if GDR_ENABLED
          if (isEncodeGdrClean)
          {
            biPDistTempOk = true;
            biPDistTempOk                     = amvp[eRefPicList].mvSolid[aaiMvpIdx[refList][refIdxTemp]];
            cMvPredSolid[refList][refIdxTemp] = biPDistTempOk;
            cMvTempSolid[refList][refIdxTemp] = biPDistTempOk;
            cMvTempValid[refList][refIdxTemp] =
              cs.isClean(pu.Y().bottomRight(), cMvTemp[refList][refIdxTemp], (RefPicList) refList, refIdxTemp);
          }
#endif

#if GDR_ENABLED
          allOk = (cs.picHeader->getMvdL1ZeroFlag() && refList == 1 && biPDistTemp < bestBiPDist);

          if (isEncodeGdrClean)
          {
            if (biPDistTempOk)
            {
              allOk = (bestBiPDistOk) ? (cs.picHeader->getMvdL1ZeroFlag() && refList == 1 && biPDistTemp < bestBiPDist)
                                      : true;
            }
            else
            {
              allOk = false;
            }
          }
#endif

#if GDR_ENABLED
          if (allOk)
#else
          if (cs.picHeader->getMvdL1ZeroFlag() && refList == 1 && biPDistTemp < bestBiPDist)
#endif
          {
            bestBiPDist = biPDistTemp;
            bestBiPMvpL1    = aaiMvpIdx[refList][refIdxTemp];
            bestBiPRefIdxL1 = refIdxTemp;
#if GDR_ENABLED
            if (isEncodeGdrClean)
            {
              bestBiPDistOk = biPDistTempOk;
            }
#endif
          }

          bitsTemp += m_auiMVPIdxCost[aaiMvpIdx[refList][refIdxTemp]][AMVP_MAX_NUM_CANDS];

          if (m_pcEncCfg->getFastMEForGenBLowDelayEnabled() && refList == 1)   // list 1
          {
            if (cs.slice->getList1IdxToList0Idx(refIdxTemp) >= 0)
            {
              cMvTemp[1][refIdxTemp] = cMvTemp[0][cs.slice->getList1IdxToList0Idx(refIdxTemp)];
#if GDR_ENABLED
              if (isEncodeGdrClean)
              {
                cMvTempSolid[1][refIdxTemp] = cMvTempSolid[1][cs.slice->getList1IdxToList0Idx(refIdxTemp)];
                cMvTempValid[1][refIdxTemp] = cs.isClean(pu.Y().bottomRight(), cMvTemp[1][refIdxTemp], (RefPicList) 1,
                                                         cs.slice->getList1IdxToList0Idx(refIdxTemp));
              }
#endif
              costTemp = uiCostTempL0[cs.slice->getList1IdxToList0Idx(refIdxTemp)];
              /*first subtract the bit-rate part of the cost of the other list*/
#if GDR_ENABLED
              if (isEncodeGdrClean)
              {
                uiCostTempOk = uiCostTempL0Ok[cs.slice->getList1IdxToList0Idx(refIdxTemp)];
              }
#endif
              costTemp -= m_pcRdCost->getCost(uiBitsTempL0[cs.slice->getList1IdxToList0Idx(refIdxTemp)]);
              /*correct the bit-rate part of the current ref*/
              m_pcRdCost->setPredictor(cMvPred[refList][refIdxTemp]);
              bitsTemp += m_pcRdCost->getBitsOfVectorWithPredictor(
                cMvTemp[1][refIdxTemp].getHor(), cMvTemp[1][refIdxTemp].getVer(), imvShift + MV_FRACTIONAL_BITS_DIFF);
              /*calculate the correct cost*/
              costTemp += m_pcRdCost->getCost(bitsTemp);
            }
            else
            {
#if GDR_ENABLED
              bCleanCandExist = false;
              xMotionEstimation(pu, origBuf, eRefPicList, cMvPred[refList][refIdxTemp], refIdxTemp,
                                cMvTemp[refList][refIdxTemp], cMvTempSolid[refList][refIdxTemp],
                                aaiMvpIdx[refList][refIdxTemp], bitsTemp, costTemp, amvp[eRefPicList], bCleanCandExist);
#else
              xMotionEstimation(pu, origBuf, eRefPicList, cMvPred[refList][refIdxTemp], refIdxTemp,
                                cMvTemp[refList][refIdxTemp], aaiMvpIdx[refList][refIdxTemp], bitsTemp, costTemp,
                                amvp[eRefPicList]);
#endif

#if GDR_ENABLED
              if (isEncodeGdrClean)
              {
                int mvpIdx                        = aaiMvpIdx[refList][refIdxTemp];
                cMvPredSolid[refList][refIdxTemp] = amvp[eRefPicList].mvSolid[mvpIdx];
                cMvTempSolid[refList][refIdxTemp] = amvp[eRefPicList].mvSolid[mvpIdx];
                cMvTempValid[refList][refIdxTemp] =
                  cs.isClean(pu.Y().bottomRight(), cMvTemp[refList][refIdxTemp], (RefPicList) refList, refIdxTemp);

                if (cMvTempValid[refList][refIdxTemp])
                {
                  cMvTempValid[refList][refIdxTemp] = cMvTempSolid[refList][refIdxTemp];
                }

                uiCostTempOk = bCleanCandExist;
                uiCostTempOk = uiCostTempOk && cMvPredSolid[refList][refIdxTemp];
                uiCostTempOk = uiCostTempOk && cMvTempSolid[refList][refIdxTemp];
                uiCostTempOk = uiCostTempOk && cMvTempValid[refList][refIdxTemp];
              }
#endif
            }
          }
          else
          {
#if GDR_ENABLED
            bCleanCandExist = false;
            xMotionEstimation(pu, origBuf, eRefPicList, cMvPred[refList][refIdxTemp], refIdxTemp,
                              cMvTemp[refList][refIdxTemp], cMvTempSolid[refList][refIdxTemp],
                              aaiMvpIdx[refList][refIdxTemp], bitsTemp, costTemp, amvp[eRefPicList], bCleanCandExist);
#else
            xMotionEstimation(pu, origBuf, eRefPicList, cMvPred[refList][refIdxTemp], refIdxTemp,
                              cMvTemp[refList][refIdxTemp], aaiMvpIdx[refList][refIdxTemp], bitsTemp, costTemp,
                              amvp[eRefPicList]);
#endif

#if GDR_ENABLED
            if (isEncodeGdrClean)
            {
              int mvpIdx                        = aaiMvpIdx[refList][refIdxTemp];
              cMvPredSolid[refList][refIdxTemp] = amvp[eRefPicList].mvSolid[mvpIdx];
              cMvTempSolid[refList][refIdxTemp] = amvp[eRefPicList].mvSolid[mvpIdx];
              cMvTempValid[refList][refIdxTemp] =
                cs.isClean(pu.Y().bottomRight(), cMvTemp[refList][refIdxTemp], (RefPicList) refList, refIdxTemp);
              if (cMvTempValid[refList][refIdxTemp])
              {
                cMvTempValid[refList][refIdxTemp] = cMvTempSolid[refList][refIdxTemp];
              }

              uiCostTempOk = bCleanCandExist;
              uiCostTempOk = uiCostTempOk && cMvPredSolid[refList][refIdxTemp];
              uiCostTempOk = uiCostTempOk && cMvTempSolid[refList][refIdxTemp];
              uiCostTempOk = uiCostTempOk && cMvTempValid[refList][refIdxTemp];
            }
#endif
          }
          if( cu.cs->sps->getUseBcw() && cu.BcwIdx == BCW_DEFAULT && cu.cs->slice->isInterB() )
          {
            const bool checkIdentical = true;
            m_uniMotions.setReadMode(checkIdentical, (uint32_t) refList, (uint32_t) refIdxTemp);
#if GDR_ENABLED
            m_uniMotions.copyFrom(cMvTemp[refList][refIdxTemp], cMvTempSolid[refList][refIdxTemp],
                                  costTemp - m_pcRdCost->getCost(bitsTemp), (uint32_t) refList, (uint32_t) refIdxTemp);
#else
            m_uniMotions.copyFrom(cMvTemp[refList][refIdxTemp], costTemp - m_pcRdCost->getCost(bitsTemp),
                                  (uint32_t) refList, (uint32_t) refIdxTemp);
#endif
          }
          xCopyAMVPInfo(&amvp[eRefPicList],
                        &aacAMVPInfo[refList][refIdxTemp]);   // must always be done ( also when AMVP_MODE = AM_NONE )
#if GDR_ENABLED
          xCheckBestMVP(pu, eRefPicList, cMvTemp[refList][refIdxTemp], cMvPred[refList][refIdxTemp],
                        aaiMvpIdx[refList][refIdxTemp], amvp[eRefPicList], bitsTemp, costTemp, pu.cu->imv);

          if (isEncodeGdrClean)
          {
            int mvpIdx = aaiMvpIdx[refList][refIdxTemp];

            cMvPredSolid[refList][refIdxTemp] = amvp[eRefPicList].mvSolid[mvpIdx];
            cMvTempSolid[refList][refIdxTemp] = amvp[eRefPicList].mvSolid[mvpIdx];
            cMvTempValid[refList][refIdxTemp] =
              cs.isClean(pu.Y().bottomRight(), cMvTemp[refList][refIdxTemp], (RefPicList) refList, refIdxTemp);
            if (cMvTempValid[refList][refIdxTemp])
            {
              cMvTempValid[refList][refIdxTemp] = cMvTempSolid[refList][refIdxTemp];
            }

            uiCostTempOk = true;
            uiCostTempOk = uiCostTempOk && cMvPredSolid[refList][refIdxTemp];
            uiCostTempOk = uiCostTempOk && cMvTempSolid[refList][refIdxTemp];
            uiCostTempOk = uiCostTempOk && cMvTempValid[refList][refIdxTemp];
          }
#else
          xCheckBestMVP(eRefPicList, cMvTemp[refList][refIdxTemp], cMvPred[refList][refIdxTemp],
                        aaiMvpIdx[refList][refIdxTemp], amvp[eRefPicList], bitsTemp, costTemp, pu.cu->imv);
#endif
          if (refList == 0)
          {
            uiCostTempL0[refIdxTemp] = costTemp;
            uiBitsTempL0[refIdxTemp] = bitsTemp;
          }
#if GDR_ENABLED
          if (isEncodeGdrClean)
          {
            uiCostTempL0Ok[refIdxTemp] = uiCostTempOk;
          }
#endif

#if GDR_ENABLED
          allOk = (costTemp < uiCost[refList]);
          if (isEncodeGdrClean)
          {
            if (uiCostTempOk)
            {
              allOk = (uiCostOk[refList]) ? (costTemp < uiCost[refList]) : true;
            }
            else
            {
              allOk = false;
            }

            allOk = allOk && bCleanCandExist;
          }
#endif


#if GDR_ENABLED
          if (allOk)
#else
          if (costTemp < uiCost[refList])
#endif
          {
            uiCost[refList] = costTemp;
            bits[refList]   = bitsTemp;   // storing for bi-prediction

            // set motion
            cMv[refList]    = cMvTemp[refList][refIdxTemp];
            refIdx[refList] = refIdxTemp;

#if GDR_ENABLED
            if (isEncodeGdrClean)
            {
              uiCostOk[refList] = uiCostTempOk;
              cMvSolid[refList] = cMvTempSolid[refList][refIdxTemp];
              cMvValid[refList] = cs.isClean(pu.Y().bottomRight(), cMv[refList], (RefPicList) refList, refIdx[refList]);
            }
#endif
          }


#if GDR_ENABLED
          allOk = (refList == 1 && costTemp < costValidList1 && cs.slice->getList1IdxToList0Idx(refIdxTemp) < 0);
          if (isEncodeGdrClean)
          {
            if (uiCostTempOk)
            {
              allOk = (costValidList1Ok)
                        ? (refList == 1 && costTemp < costValidList1 && cs.slice->getList1IdxToList0Idx(refIdxTemp) < 0)
                        : true;
            }
            else
            {
              allOk = false;
            }
          }
#endif

#if GDR_ENABLED
          if (allOk)
#else
          if (refList == 1 && costTemp < costValidList1 && cs.slice->getList1IdxToList0Idx(refIdxTemp) < 0)
#endif
          {
            costValidList1 = costTemp;
            bitsValidList1 = bitsTemp;

            // set motion
            mvValidList1 = cMvTemp[refList][refIdxTemp];
#if GDR_ENABLED
            if (isEncodeGdrClean)
            {
              costValidList1Ok = uiCostTempOk;
              mvValidList1Solid = cMvTempSolid[refList][refIdxTemp];
              mvValidList1Valid = cs.isClean(pu.Y().bottomRight(), mvValidList1, (RefPicList) refList, refIdxTemp);
            }
#endif
            refIdxValidList1 = refIdxTemp;
          }
        }
      }

      ::memcpy(cMvHevcTemp, cMvTemp, sizeof(cMvTemp));
#if GDR_ENABLED
      if (isEncodeGdrClean)
      {
        ::memcpy(cMvHevcTempSolid, cMvTempSolid, sizeof(cMvTempSolid));
        ::memcpy(cMvHevcTempValid, cMvTempValid, sizeof(cMvTempValid));
      }
#endif
      if (cu.imv == 0 && (!cu.slice->getSPS()->getUseBcw() || bcwIdx == BCW_DEFAULT))
      {
        insertUniMvCands(pu.Y(), cMvTemp);

        unsigned idx1, idx2, idx3, idx4;
        getAreaIdx(cu.Y(), *cu.slice->getPPS()->pcv, idx1, idx2, idx3, idx4);
        ::memcpy(&(g_reusedUniMVs[idx1][idx2][idx3][idx4][0][0]), cMvTemp, 2 * 33 * sizeof(Mv));
        g_isReusedUniMVsFilled[idx1][idx2][idx3][idx4] = true;
      }

      //RAMIRO: Predicao Bidirecional
      //  Bi-predictive Motion estimation
      if( ( cs.slice->isInterB() ) && ( PU::isBipredRestriction( pu ) == false )
        && (cu.slice->getCheckLDC() || bcwIdx == BCW_DEFAULT || !m_affineModeSelected || !m_pcEncCfg->getUseBcwFast())
        )
      {
        bool doBiPred = true;
        tryBipred = 1;
        cMvBi[0] = cMv[0];
        cMvBi[1] = cMv[1];
        iRefIdxBi[0]  = refIdx[0];
        iRefIdxBi[1]  = refIdx[1];

#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          cMvBiSolid[0] = cMvSolid[0];
          cMvBiSolid[1] = cMvSolid[1];
          cMvBiValid[0] = cMvValid[0];
          cMvBiValid[1] = cMvValid[1];
        }
#endif
        ::memcpy( cMvPredBi,   cMvPred,   sizeof( cMvPred   ) );
        ::memcpy( aaiMvpIdxBi, aaiMvpIdx, sizeof( aaiMvpIdx ) );
#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          ::memcpy(cMvPredBiSolid, cMvPredSolid, sizeof(cMvPredSolid));
        }
#endif

        uint32_t motBits[2];

        if(cs.picHeader->getMvdL1ZeroFlag())
        {
          xCopyAMVPInfo(&aacAMVPInfo[1][bestBiPRefIdxL1], &amvp[REF_PIC_LIST_1]);
          aaiMvpIdxBi[1][bestBiPRefIdxL1] = bestBiPMvpL1;
          cMvPredBi  [1][bestBiPRefIdxL1] = amvp[REF_PIC_LIST_1].mvCand[bestBiPMvpL1];

          cMvBi    [1] = cMvPredBi[1][bestBiPRefIdxL1];
#if GDR_ENABLED
          if (isEncodeGdrClean)
          {
            cMvPredBiSolid[1][bestBiPRefIdxL1] = amvp[REF_PIC_LIST_1].mvSolid[bestBiPMvpL1];
            cMvBiSolid[1] = cMvPredBiSolid[1][bestBiPRefIdxL1];
            cMvBiValid[1] = cs.isClean(pu.Y().bottomRight(), cMvBi[1], REF_PIC_LIST_1, bestBiPRefIdxL1);
          }
#endif
          iRefIdxBi[1] = bestBiPRefIdxL1;
          pu.mv    [REF_PIC_LIST_1] = cMvBi[1];
          pu.refIdx[REF_PIC_LIST_1] = iRefIdxBi[1];
          pu.mvpIdx[REF_PIC_LIST_1] = bestBiPMvpL1;
#if GDR_ENABLED
          if (isEncodeGdrClean)
          {
            pu.mvSolid[REF_PIC_LIST_1] = cMvBiSolid[1];
            pu.mvValid[REF_PIC_LIST_1] = cs.isClean(pu.Y().bottomRight(), pu.mv[REF_PIC_LIST_1], REF_PIC_LIST_1, pu.refIdx[REF_PIC_LIST_1]);
          }
#endif

          if( m_pcEncCfg->getMCTSEncConstraint() )
          {
            Mv restrictedMv = pu.mv[REF_PIC_LIST_1];
            Area curTileAreaRestricted;
            curTileAreaRestricted = pu.cs->picture->mctsInfo.getTileAreaSubPelRestricted( pu );
            MCTSHelper::clipMvToArea( restrictedMv, pu.cu->Y(), curTileAreaRestricted, *pu.cs->sps );
            // If sub-pel filter samples are not inside of allowed area
            if( restrictedMv != pu.mv[REF_PIC_LIST_1] )
            {
              uiCostBi = std::numeric_limits<Distortion>::max();
              doBiPred = false;
            }
          }
          PelUnitBuf predBufTmp = m_tmpPredStorage[REF_PIC_LIST_1].getBuf( UnitAreaRelative(cu, pu) );
          motionCompensation( pu, predBufTmp, REF_PIC_LIST_1 );

          motBits[0] = bits[0] - mbBits[0];
          motBits[1] = mbBits[1];

          if ( cs.slice->getNumRefIdx(REF_PIC_LIST_1) > 1 )
          {
            motBits[1] += bestBiPRefIdxL1 + 1;
            if ( bestBiPRefIdxL1 == cs.slice->getNumRefIdx(REF_PIC_LIST_1)-1 )
            {
              motBits[1]--;
            }
          }

          motBits[1] += m_auiMVPIdxCost[aaiMvpIdxBi[1][bestBiPRefIdxL1]][AMVP_MAX_NUM_CANDS];

          bits[2] = mbBits[2] + motBits[0] + motBits[1];

          cMvTemp[1][bestBiPRefIdxL1] = cMvBi[1];
#if GDR_ENABLED
          if (isEncodeGdrClean)
          {
            cMvTempSolid[1][bestBiPRefIdxL1] = cMvBiSolid[1];
            cMvTempValid[1][bestBiPRefIdxL1] = cs.isClean(pu.Y().bottomRight(), cMvBi[1], REF_PIC_LIST_1, bestBiPRefIdxL1);
          }
#endif
        }
        else
        {
          motBits[0] = bits[0] - mbBits[0];
          motBits[1] = bits[1] - mbBits[1];
          bits[2]    = mbBits[2] + motBits[0] + motBits[1];
        }

        if( doBiPred )
        {
          // 4-times iteration (default)
          int numIter = 4;

          // fast encoder setting: only one iteration
          if (m_pcEncCfg->getFastInterSearchMode() == FASTINTERSEARCH_MODE1
              || m_pcEncCfg->getFastInterSearchMode() == FASTINTERSEARCH_MODE2 || cs.picHeader->getMvdL1ZeroFlag())
          {
            numIter = 1;
          }

          enforceBcwPred = (bcwIdx != BCW_DEFAULT);
          for (int iter = 0; iter < numIter; iter++)
          {
            int refList = iter % 2;

            if (m_pcEncCfg->getFastInterSearchMode() == FASTINTERSEARCH_MODE1
                || m_pcEncCfg->getFastInterSearchMode() == FASTINTERSEARCH_MODE2)
            {
#if GDR_ENABLED
              allOk = (uiCost[0] <= uiCost[1]);
              if (isEncodeGdrClean)
              {
                if (uiCostOk[0])
                {
                  allOk = (uiCostOk[1]) ? (uiCost[0] <= uiCost[1]) : true;
                }
                else
                {
                  allOk = false;
                }
              }
#endif

#if GDR_ENABLED
              if (allOk)
#else
              if (uiCost[0] <= uiCost[1])
#endif
              {
                refList = 1;
              }
              else
              {
                refList = 0;
              }
              if (bcwIdx != BCW_DEFAULT)
              {
                refList =
                  (abs(getBcwWeight(bcwIdx, REF_PIC_LIST_0)) > abs(getBcwWeight(bcwIdx, REF_PIC_LIST_1)) ? 1 : 0);
              }
            }
            else if (iter == 0)
            {
              refList = 0;
            }
            if (iter == 0 && !cs.picHeader->getMvdL1ZeroFlag())
            {
              pu.mv[1 - refList]     = cMv[1 - refList];
              pu.refIdx[1 - refList] = refIdx[1 - refList];
#if GDR_ENABLED
              if (isEncodeGdrClean)
              {
                pu.mvSolid[1 - refList] = cMvSolid[1 - refList];
                pu.mvValid[1 - refList] = cs.isClean(pu.Y().bottomRight(), pu.mv[1 - refList],
                                                     (RefPicList) (1 - refList), pu.refIdx[1 - refList]);
              }
#endif
              PelUnitBuf predBufTmp = m_tmpPredStorage[1 - refList].getBuf(UnitAreaRelative(cu, pu));
              motionCompensation(pu, predBufTmp, RefPicList(1 - refList));
            }

            RefPicList eRefPicList = (refList ? REF_PIC_LIST_1 : REF_PIC_LIST_0);

            if (cs.picHeader->getMvdL1ZeroFlag())
            {
              refList     = 0;
              eRefPicList = REF_PIC_LIST_0;
            }

            bool changed = false;

            iRefStart = 0;
            iRefEnd   = cs.slice->getNumRefIdx(eRefPicList) - 1;
            for (int refIdxTemp = iRefStart; refIdxTemp <= iRefEnd; refIdxTemp++)
            {
              if (m_pcEncCfg->getUseBcwFast() && (bcwIdx != BCW_DEFAULT)
                  && (pu.cu->slice->getRefPic(eRefPicList, refIdxTemp)->getPOC()
                      == pu.cu->slice->getRefPic(RefPicList(1 - refList), pu.refIdx[1 - refList])->getPOC())
                  && (!pu.cu->imv && pu.cu->slice->getTLayer() > 1))
              {
                continue;
              }
              bitsTemp = mbBits[2] + motBits[1 - refList];
              bitsTemp += ((cs.slice->getSPS()->getUseBcw() == true) ? getWeightIdxBits(bcwIdx) : 0);
              if (cs.slice->getNumRefIdx(eRefPicList) > 1)
              {
                bitsTemp += refIdxTemp + 1;
                if (refIdxTemp == cs.slice->getNumRefIdx(eRefPicList) - 1)
                {
                  bitsTemp--;
                }
              }
              bitsTemp += m_auiMVPIdxCost[aaiMvpIdxBi[refList][refIdxTemp]][AMVP_MAX_NUM_CANDS];
              if (cs.slice->getBiDirPred())
              {
                bitsTemp += 1;   // add one bit for symmetrical MVD mode
              }
              // call ME
              xCopyAMVPInfo(&aacAMVPInfo[refList][refIdxTemp], &amvp[eRefPicList]);
#if GDR_ENABLED
              bCleanCandExist = false;
              xMotionEstimation(pu, origBuf, eRefPicList, cMvPredBi[refList][refIdxTemp], refIdxTemp,
                                cMvTemp[refList][refIdxTemp], cMvTempSolid[refList][refIdxTemp],
                                aaiMvpIdxBi[refList][refIdxTemp], bitsTemp, costTemp, amvp[eRefPicList],
                                bCleanCandExist, true);
#else
              xMotionEstimation(pu, origBuf, eRefPicList, cMvPredBi[refList][refIdxTemp], refIdxTemp,
                                cMvTemp[refList][refIdxTemp], aaiMvpIdxBi[refList][refIdxTemp], bitsTemp, costTemp,
                                amvp[eRefPicList], true);
#endif
#if GDR_ENABLED
              if (isEncodeGdrClean)
              {
                int mvpIdx                          = aaiMvpIdxBi[refList][refIdxTemp];
                cMvPredBiSolid[refList][refIdxTemp] = amvp[eRefPicList].mvSolid[mvpIdx];
                cMvTempSolid[refList][refIdxTemp]   = amvp[eRefPicList].mvSolid[mvpIdx];
                cMvTempValid[refList][refIdxTemp] =
                  cs.isClean(pu.Y().bottomRight(), cMvTemp[refList][refIdxTemp], (RefPicList) refList, refIdxTemp);
                if (cMvTempValid[refList][refIdxTemp])
                {
                  cMvTempValid[refList][refIdxTemp] = cMvTempSolid[refList][refIdxTemp];
                }

                uiCostTempOk = bCleanCandExist;
                uiCostTempOk = uiCostTempOk && cMvPredBiSolid[refList][refIdxTemp];
                uiCostTempOk = uiCostTempOk && cMvTempSolid[refList][refIdxTemp];
                uiCostTempOk = uiCostTempOk && cMvTempValid[refList][refIdxTemp];
              }
#endif

#if GDR_ENABLED
              // note : costTemp is the new Best MVP cost,
              //        solid info will be at amvp[eRefPicList].mvSolid[aaiMvpIdx[refList][refIdxTemp]];
              xCheckBestMVP(pu, eRefPicList, cMvTemp[refList][refIdxTemp], cMvPredBi[refList][refIdxTemp],
                            aaiMvpIdxBi[refList][refIdxTemp], amvp[eRefPicList], bitsTemp, costTemp, pu.cu->imv);

              if (isEncodeGdrClean)
              {
                int mvpIdx = aaiMvpIdxBi[refList][refIdxTemp];

                cMvPredBiSolid[refList][refIdxTemp] = amvp[eRefPicList].mvSolid[mvpIdx];
                cMvTempSolid[refList][refIdxTemp]   = amvp[eRefPicList].mvSolid[mvpIdx];
                cMvTempValid[refList][refIdxTemp] =
                  cs.isClean(pu.Y().bottomRight(), cMvTemp[refList][refIdxTemp], (RefPicList) refList, refIdxTemp);
                if (cMvTempValid[refList][refIdxTemp])
                {
                  cMvTempValid[refList][refIdxTemp] = cMvTempSolid[refList][refIdxTemp];
                }

                uiCostTempOk = true;
                uiCostTempOk = uiCostTempOk && cMvPredBiSolid[refList][refIdxTemp];
                uiCostTempOk = uiCostTempOk && cMvTempSolid[refList][refIdxTemp];
                uiCostTempOk = uiCostTempOk && cMvTempValid[refList][refIdxTemp];
              }
#else

              xCheckBestMVP(eRefPicList, cMvTemp[refList][refIdxTemp], cMvPredBi[refList][refIdxTemp],
                            aaiMvpIdxBi[refList][refIdxTemp], amvp[eRefPicList], bitsTemp, costTemp, pu.cu->imv);
#endif
#if GDR_ENABLED
              allOk = (costTemp < uiCostBi);
              if (isEncodeGdrClean)
              {
                if (uiCostTempOk)
                {
                  allOk = (uiCostBiOk) ? (costTemp < uiCostBi) : true;
                }
                else
                {
                  allOk = false;
                }
              }
#endif
#if GDR_ENABLED
              if (allOk)
#else
              if (costTemp < uiCostBi)
#endif
              {
                changed = true;

                cMvBi[refList] = cMvTemp[refList][refIdxTemp];
#if GDR_ENABLED
                if (isEncodeGdrClean)
                {
                  cMvBiSolid[refList] = cMvTempSolid[refList][refIdxTemp];
                  cMvBiValid[refList] =
                    cs.isClean(pu.Y().bottomRight(), cMvTemp[refList][refIdxTemp], (RefPicList) refList, refIdxTemp);
                }
#endif
                iRefIdxBi[refList] = refIdxTemp;

                uiCostBi = costTemp;
#if GDR_ENABLED
                if (isEncodeGdrClean)
                {
                  uiCostBiOk = uiCostTempOk;
                }
#endif
                motBits[refList] = bitsTemp - mbBits[2] - motBits[1 - refList];
                motBits[refList] -= ((cs.slice->getSPS()->getUseBcw() == true) ? getWeightIdxBits(bcwIdx) : 0);
                bits[2] = bitsTemp;

                if (numIter != 1)
                {
                  //  Set motion
                  pu.mv[eRefPicList]     = cMvBi[refList];
                  pu.refIdx[eRefPicList] = iRefIdxBi[refList];
#if GDR_ENABLED
                  if (isEncodeGdrClean)
                  {
                    pu.mvSolid[eRefPicList] = cMvBiSolid[refList];
                    pu.mvValid[eRefPicList] = cs.isClean(pu.Y().bottomRight(), pu.mv[eRefPicList], (RefPicList)eRefPicList, pu.refIdx[eRefPicList]);
                  }
#endif
                  PelUnitBuf predBufTmp = m_tmpPredStorage[refList].getBuf(UnitAreaRelative(cu, pu));
                  motionCompensation(pu, predBufTmp, eRefPicList);
                }
              }
            }   // for loop-refIdxTemp

            if (!changed)
            {
#if GDR_ENABLED
              allOk = ((uiCostBi <= uiCost[0] && uiCostBi <= uiCost[1]) || enforceBcwPred);

              if (isEncodeGdrClean)
              {
                if (uiCostBiOk)
                {
                  allOk = (uiCostOk[0] && uiCostOk[1]) ? ((uiCostBi <= uiCost[0] && uiCostBi <= uiCost[1]) || enforceBcwPred) : true;
                }
                else
                {
                  allOk = false;
                }
              }
#endif
#if GDR_ENABLED
              if (allOk)
#else
              if ((uiCostBi <= uiCost[0] && uiCostBi <= uiCost[1]) || enforceBcwPred)
#endif
              {
                xCopyAMVPInfo(&aacAMVPInfo[0][iRefIdxBi[0]], &amvp[REF_PIC_LIST_0]);
#if GDR_ENABLED
                // note : uiCostBi is the new Best MVP cost,
                //          solid info will be at amvp[eRefPicList].mvSolid[aaiMvpIdx[refList][refIdxTemp]];
                xCheckBestMVP(pu, REF_PIC_LIST_0, cMvBi[0], cMvPredBi[0][iRefIdxBi[0]], aaiMvpIdxBi[0][iRefIdxBi[0]],
                              amvp[REF_PIC_LIST_0], bits[2], uiCostBi, pu.cu->imv);

                if (isEncodeGdrClean)
                {
                  int mvpIdx = aaiMvpIdxBi[0][iRefIdxBi[0]];

                  cMvPredBiSolid[0][iRefIdxBi[0]] = amvp[0].mvSolid[mvpIdx];
                  cMvBiSolid[0]                   = amvp[0].mvSolid[mvpIdx];
                  cMvBiValid[0] = cs.isClean(pu.Y().bottomRight(), cMvBi[0], (RefPicList)0, iRefIdxBi[0]);
                  if (cMvBiValid[0])
                  {
                    cMvBiValid[0] = cMvBiSolid[0];
                  }

                  uiCostBiOk = true;
                  uiCostBiOk = uiCostBiOk && cMvPredBiSolid[0][iRefIdxBi[0]];
                  uiCostBiOk = uiCostBiOk && cMvBiSolid[0];
                  uiCostBiOk = uiCostBiOk && cMvBiValid[0];
                }
#else
                xCheckBestMVP(REF_PIC_LIST_0, cMvBi[0], cMvPredBi[0][iRefIdxBi[0]], aaiMvpIdxBi[0][iRefIdxBi[0]],
                              amvp[REF_PIC_LIST_0], bits[2], uiCostBi, pu.cu->imv);
#endif
                if (!cs.picHeader->getMvdL1ZeroFlag())
                {
                  xCopyAMVPInfo(&aacAMVPInfo[1][iRefIdxBi[1]], &amvp[REF_PIC_LIST_1]);
#if GDR_ENABLED
                  // note : uiCostBi is the new Best MVP cost,
                  //          solid info will be at amvp[eRefPicList].mvSolid[aaiMvpIdx[refList][refIdxTemp]];
                  xCheckBestMVP(pu, REF_PIC_LIST_1, cMvBi[1], cMvPredBi[1][iRefIdxBi[1]], aaiMvpIdxBi[1][iRefIdxBi[1]],
                                amvp[REF_PIC_LIST_1], bits[2], uiCostBi, pu.cu->imv);

                  if (isEncodeGdrClean)
                  {
                    int mvpIdx = aaiMvpIdxBi[1][iRefIdxBi[1]];

                    cMvPredBiSolid[1][iRefIdxBi[1]] = aaiMvpIdxBi[1][iRefIdxBi[1]];
                    cMvBiSolid[1]                   = amvp[REF_PIC_LIST_1].mvSolid[mvpIdx];
                    cMvBiValid[1] = cs.isClean(pu.Y().bottomRight(), cMvBi[1], (RefPicList)1, iRefIdxBi[1]);
                    if (cMvBiValid[1])
                    {
                      cMvBiValid[1] = cMvBiSolid[1];
                    }

                    uiCostBiOk = true;
                    uiCostBiOk = uiCostBiOk && cMvPredBiSolid[1][iRefIdxBi[1]];
                    uiCostBiOk = uiCostBiOk && cMvBiSolid[1];
                    uiCostBiOk = uiCostBiOk && cMvBiValid[1];
                  }
#else
                  xCheckBestMVP(REF_PIC_LIST_1, cMvBi[1], cMvPredBi[1][iRefIdxBi[1]], aaiMvpIdxBi[1][iRefIdxBi[1]],
                                amvp[REF_PIC_LIST_1], bits[2], uiCostBi, pu.cu->imv);
#endif
                }
              }
              break;
            }
          }   // for loop-iter
        }
        cu.refIdxBi[0] = iRefIdxBi[0];
        cu.refIdxBi[1] = iRefIdxBi[1];

        if ( cs.slice->getBiDirPred() && trySmvd )
        {
          Distortion symCost;
          Mv cMvPredSym[2];
          int mvpIdxSym[2];

          int curRefList = REF_PIC_LIST_0;
          int tarRefList = 1 - curRefList;
          RefPicList eCurRefList = (curRefList ? REF_PIC_LIST_1 : REF_PIC_LIST_0);
          int refIdxCur = cs.slice->getSymRefIdx( curRefList );
          int refIdxTar = cs.slice->getSymRefIdx( tarRefList );
          CHECK (refIdxCur==-1 || refIdxTar==-1, "Uninitialized reference index not allowed");

          if ( aacAMVPInfo[curRefList][refIdxCur].mvCand[0] == aacAMVPInfo[curRefList][refIdxCur].mvCand[1] )
          {
            aacAMVPInfo[curRefList][refIdxCur].numCand = 1;
          }
          if ( aacAMVPInfo[tarRefList][refIdxTar].mvCand[0] == aacAMVPInfo[tarRefList][refIdxTar].mvCand[1] )
          {
            aacAMVPInfo[tarRefList][refIdxTar].numCand = 1;
          }

          MvField cCurMvField, cTarMvField;
          Distortion costStart = std::numeric_limits<Distortion>::max();

#if GDR_ENABLED
          bool cMvPredSymSolid[2] = { init_value, init_value };
          bool cMvPredSymValid[2] = { init_value, init_value };

          bool cCurMvFieldSolid = init_value;
          bool cTarMvFieldSolid = init_value;
          bool cCurMvFieldValid = init_value;
          bool cTarMvFieldValid = init_value;

          bool costStartOk = false;
          bool symCostOk = init_value;
          bool costOk = init_value;
          bool bestCostOk = init_value;
#endif
          for ( int i = 0; i < aacAMVPInfo[curRefList][refIdxCur].numCand; i++ )
          {
            for ( int j = 0; j < aacAMVPInfo[tarRefList][refIdxTar].numCand; j++ )
            {
              cCurMvField.setMvField( aacAMVPInfo[curRefList][refIdxCur].mvCand[i], refIdxCur );
              cTarMvField.setMvField( aacAMVPInfo[tarRefList][refIdxTar].mvCand[j], refIdxTar );
              Distortion cost = xGetSymmetricCost( pu, origBuf, eCurRefList, cCurMvField, cTarMvField, bcwIdx );

#if GDR_ENABLED
              if (isEncodeGdrClean)
              {
                cCurMvFieldSolid = aacAMVPInfo[curRefList][refIdxCur].mvSolid[i];
                cTarMvFieldSolid = aacAMVPInfo[tarRefList][refIdxTar].mvSolid[i];
                costOk = cCurMvFieldSolid && cTarMvFieldSolid;
              }
#endif
#if GDR_ENABLED
              allOk = (cost < costStart);
              if (isEncodeGdrClean)
              {
                if (costOk)
                {
                  allOk = (costStartOk) ? (cost < costStart) : true;
                }
                else
                {
                  allOk = false;
                }
              }
#endif
#if GDR_ENABLED
              if (allOk)
#else
              if ( cost < costStart )
#endif
              {
                costStart = cost;
                cMvPredSym[curRefList] = aacAMVPInfo[curRefList][refIdxCur].mvCand[i];
                cMvPredSym[tarRefList] = aacAMVPInfo[tarRefList][refIdxTar].mvCand[j];
#if GDR_ENABLED
                if (isEncodeGdrClean)
                {
                  costStartOk = costOk;
                  cMvPredSymSolid[curRefList] = aacAMVPInfo[curRefList][refIdxCur].mvSolid[i];
                  cMvPredSymSolid[tarRefList] = aacAMVPInfo[tarRefList][refIdxTar].mvSolid[j];
                }
#endif
                mvpIdxSym[curRefList] = i;
                mvpIdxSym[tarRefList] = j;
              }
            }
          }
          cCurMvField.mv = cMvPredSym[curRefList];
          cTarMvField.mv = cMvPredSym[tarRefList];

#if GDR_ENABLED
          if (isEncodeGdrClean)
          {
            cCurMvFieldSolid = cMvPredSymSolid[curRefList];
            cTarMvFieldSolid = cMvPredSymSolid[tarRefList];
          }
#endif
          m_pcRdCost->setCostScale(0);
          Mv pred = cMvPredSym[curRefList];
          pred.changeTransPrecInternal2Amvr(pu.cu->imv);
          m_pcRdCost->setPredictor(pred);
          Mv mv = cCurMvField.mv;
          mv.changeTransPrecInternal2Amvr(pu.cu->imv);
          uint32_t bits = m_pcRdCost->getBitsOfVectorWithPredictor(mv.hor, mv.ver, 0);
          bits += m_auiMVPIdxCost[mvpIdxSym[curRefList]][AMVP_MAX_NUM_CANDS];
          bits += m_auiMVPIdxCost[mvpIdxSym[tarRefList]][AMVP_MAX_NUM_CANDS];
          costStart += m_pcRdCost->getCost(bits);

          std::vector<Mv> symmvdCands;
          auto smmvdCandsGen = [&](Mv mvCand, bool mvPrecAdj)
          {
            if (mvPrecAdj && pu.cu->imv)
            {
              mvCand.roundTransPrecInternal2Amvr(pu.cu->imv);
            }

            bool toAddMvCand = true;
            for (std::vector<Mv>::iterator pos = symmvdCands.begin(); pos != symmvdCands.end(); pos++)
            {
              if (*pos == mvCand)
              {
                toAddMvCand = false;
                break;
              }
            }

            if (toAddMvCand)
            {
              symmvdCands.push_back(mvCand);
            }
          };

          smmvdCandsGen(cMvHevcTemp[curRefList][refIdxCur], false);
          smmvdCandsGen(cMvTemp[curRefList][refIdxCur], false);
          if (iRefIdxBi[curRefList] == refIdxCur)
          {
            smmvdCandsGen(cMvBi[curRefList], false);
          }
          for (int i = 0; i < m_uniMvListSize; i++)
          {
            if ( symmvdCands.size() >= 5 )
            {
              break;
            }
            BlkUniMvInfo* curMvInfo = m_uniMvList + ((m_uniMvListIdx - 1 - i + m_uniMvListMaxSize) % (m_uniMvListMaxSize));
            smmvdCandsGen(curMvInfo->uniMvs[curRefList][refIdxCur], true);
          }

          for (auto mvStart : symmvdCands)
          {
            bool checked = false; //if it has been checkin in the mvPred.
            for (int i = 0; i < aacAMVPInfo[curRefList][refIdxCur].numCand && !checked; i++)
            {
              checked |= (mvStart == aacAMVPInfo[curRefList][refIdxCur].mvCand[i]);
            }
            if (checked)
            {
              continue;
            }

            Distortion bestCost = costStart;
#if GDR_ENABLED
            symmvdCheckBestMvp(pu, origBuf, mvStart, (RefPicList)curRefList, aacAMVPInfo, bcwIdx, cMvPredSym, cMvPredSymSolid, mvpIdxSym, costStart);
#else
            symmvdCheckBestMvp(pu, origBuf, mvStart, (RefPicList)curRefList, aacAMVPInfo, bcwIdx, cMvPredSym, mvpIdxSym, costStart);
#endif

#if GDR_ENABLED
            if (isEncodeGdrClean)
            {
              int mvp_idx0 = mvpIdxSym[0];
              int mvp_idx1 = mvpIdxSym[1];

              cMvPredSymSolid[curRefList] = aacAMVPInfo[curRefList][refIdxCur].mvSolid[mvp_idx0];
              cMvPredSymSolid[tarRefList] = aacAMVPInfo[tarRefList][refIdxTar].mvSolid[mvp_idx1];
              cMvPredSymValid[curRefList] = cs.isClean(pu.Y().bottomRight(), mvStart, (RefPicList)curRefList, pu.cu->slice->getSymRefIdx(curRefList));
              cMvPredSymValid[tarRefList] = cs.isClean(pu.Y().bottomRight(), mvStart, (RefPicList)tarRefList, pu.cu->slice->getSymRefIdx(tarRefList));

              costStartOk = true;
              costStartOk = costStartOk && cMvPredSymSolid[curRefList];
              costStartOk = costStartOk && cMvPredSymSolid[tarRefList];
              costStartOk = costStartOk && cMvPredSymValid[curRefList];
              costStartOk = costStartOk && cMvPredSymValid[tarRefList];
            }
#endif

#if GDR_ENABLED
            bool allOk = (costStart < bestCost);
            if (isEncodeGdrClean)
            {
              if (costStartOk)
              {
                allOk = (bestCostOk) ? (costStart < bestCost) : true;
              }
              else
              {
                allOk = false;
              }
            }
#endif

#if GDR_ENABLED
            if (allOk)
#else
            if (costStart < bestCost)
#endif
            {
              cCurMvField.setMvField(mvStart, refIdxCur);
              cTarMvField.setMvField(mvStart.getSymmvdMv(cMvPredSym[curRefList], cMvPredSym[tarRefList]), refIdxTar);
#if GDR_ENABLED
              if (isEncodeGdrClean)
              {
                cCurMvFieldSolid = cMvPredSymSolid[curRefList];
                cTarMvFieldSolid = cMvPredSymSolid[tarRefList];
                cCurMvFieldValid = cMvPredSymValid[curRefList];
                cTarMvFieldValid = cMvPredSymValid[tarRefList];
              }
#endif
            }
          }
          Mv startPtMv = cCurMvField.mv;

          Distortion mvpCost = m_pcRdCost->getCost(m_auiMVPIdxCost[mvpIdxSym[curRefList]][AMVP_MAX_NUM_CANDS] + m_auiMVPIdxCost[mvpIdxSym[tarRefList]][AMVP_MAX_NUM_CANDS]);
          symCost = costStart - mvpCost;

          // ME
#if GDR_ENABLED
          xSymmetricMotionEstimation(pu, origBuf, cMvPredSym[curRefList], cMvPredSym[tarRefList], eCurRefList, cCurMvField, cTarMvField, symCost, bcwIdx, costStartOk);
#else
          xSymmetricMotionEstimation( pu, origBuf, cMvPredSym[curRefList], cMvPredSym[tarRefList], eCurRefList, cCurMvField, cTarMvField, symCost, bcwIdx );
#endif
          symCost += mvpCost;

#if GDR_ENABLED
          if (isEncodeGdrClean)
          {
            cCurMvFieldValid = cs.isClean(pu.Y().bottomRight(), cCurMvField.mv, (RefPicList)(eCurRefList), cCurMvField.refIdx);
            cTarMvFieldValid = cs.isClean(pu.Y().bottomRight(), cTarMvField.mv, (RefPicList)(1 - eCurRefList), cTarMvField.refIdx);
            symCostOk = (cMvPredSymSolid[curRefList] && cMvPredSymSolid[tarRefList]) && (cCurMvFieldValid && cTarMvFieldValid);
          }
#endif
          if (startPtMv != cCurMvField.mv)
          { // if ME change MV, run a final check for best MVP.
#if GDR_ENABLED
            symmvdCheckBestMvp(pu, origBuf, cCurMvField.mv, (RefPicList)curRefList, aacAMVPInfo, bcwIdx, cMvPredSym, cMvPredSymSolid, mvpIdxSym, symCost);
#else
            symmvdCheckBestMvp(pu, origBuf, cCurMvField.mv, (RefPicList)curRefList, aacAMVPInfo, bcwIdx, cMvPredSym, mvpIdxSym, symCost, true);
#endif

#if GDR_ENABLED
            if (isEncodeGdrClean)
            {
              int mvp_idx0 = mvpIdxSym[0];
              int mvp_idx1 = mvpIdxSym[1];

              cMvPredSymSolid[curRefList] = aacAMVPInfo[curRefList][refIdxCur].mvSolid[mvp_idx0];
              cMvPredSymSolid[tarRefList] = aacAMVPInfo[tarRefList][refIdxTar].mvSolid[mvp_idx1];
              cMvPredSymValid[curRefList] = cs.isClean(pu.Y().bottomRight(), cCurMvField.mv, (RefPicList)curRefList, pu.cu->slice->getSymRefIdx(curRefList));
              cMvPredSymValid[tarRefList] = cs.isClean(pu.Y().bottomRight(), cCurMvField.mv, (RefPicList)tarRefList, pu.cu->slice->getSymRefIdx(tarRefList));

              symCostOk = true;
              symCostOk = symCostOk && cMvPredSymSolid[curRefList];
              symCostOk = symCostOk && cMvPredSymSolid[tarRefList];
              symCostOk = symCostOk && cMvPredSymValid[curRefList];
              symCostOk = symCostOk && cMvPredSymValid[tarRefList];
            }
#endif
          }

          bits = mbBits[2];
          bits += 1; // add one bit for #symmetrical MVD mode
          bits += ((cs.slice->getSPS()->getUseBcw() == true) ? getWeightIdxBits(bcwIdx) : 0);
          symCost += m_pcRdCost->getCost(bits);
          cTarMvField.setMvField(cCurMvField.mv.getSymmvdMv(cMvPredSym[curRefList], cMvPredSym[tarRefList]), refIdxTar);

          if( m_pcEncCfg->getMCTSEncConstraint() )
          {
            if( !( MCTSHelper::checkMvForMCTSConstraint( pu, cCurMvField.mv ) && MCTSHelper::checkMvForMCTSConstraint( pu, cTarMvField.mv ) ) )
            {
              symCost = std::numeric_limits<Distortion>::max();
            }
          }
          // save results
#if GDR_ENABLED
          bool allOk = (symCost < uiCostBi);
          if (isEncodeGdrClean)
          {
            if (symCostOk)
            {
              allOk = (uiCostBiOk) ? (symCost < uiCostBi) : true;
            }
            else
            {
              allOk = false;
            }
          }
#endif

#if GDR_ENABLED
          if (allOk)
#else
          if ( symCost < uiCostBi )
#endif
          {
            uiCostBi = symCost;
            symMode = 1 + curRefList;
#if GDR_ENABLED
            uiCostBiOk = symCostOk;
#endif

            cMvBi[curRefList] = cCurMvField.mv;
            iRefIdxBi[curRefList] = cCurMvField.refIdx;
            aaiMvpIdxBi[curRefList][cCurMvField.refIdx] = mvpIdxSym[curRefList];
            cMvPredBi[curRefList][iRefIdxBi[curRefList]] = cMvPredSym[curRefList];

            cMvBi[tarRefList] = cTarMvField.mv;
            iRefIdxBi[tarRefList] = cTarMvField.refIdx;
            aaiMvpIdxBi[tarRefList][cTarMvField.refIdx] = mvpIdxSym[tarRefList];
            cMvPredBi[tarRefList][iRefIdxBi[tarRefList]] = cMvPredSym[tarRefList];

#if GDR_ENABLED
            if (isEncodeGdrClean)
            {
              cMvBiValid[curRefList] = cCurMvFieldValid;
              cMvBiValid[tarRefList] = cTarMvFieldValid;
              cMvPredBiSolid[curRefList][iRefIdxBi[curRefList]] = cMvPredSymSolid[curRefList];
              cMvPredBiSolid[tarRefList][iRefIdxBi[tarRefList]] = cMvPredSymSolid[tarRefList];
            }
#endif
          }
        }
      } // if (B_SLICE)

//RAMIRO: função para calcular e extrair features
#if EXTRA_FEATURES
    if(pu.cu->affineType == AFFINEMODEL_4PARAM){    //VTM 16.2
    //if (pu.cu->affineType == AffineModel::_4_PARAMS){ //VTM 22.0
        affCustoMvUniL0_1 = uiCost[0];
        affCustoMvUniL1_1 = uiCost[1];
        affbitsMvUniL0 = bits[0];
        affBitsMvUniL1 = bits[1];
    }
#endif

      //  Clear Motion Field
      pu.mv[REF_PIC_LIST_0]     = Mv();
      pu.mv[REF_PIC_LIST_1]     = Mv();
      pu.mvd[REF_PIC_LIST_0]    = cMvZero;
      pu.mvd[REF_PIC_LIST_1]    = cMvZero;
      pu.refIdx[REF_PIC_LIST_0] = NOT_VALID;
      pu.refIdx[REF_PIC_LIST_1] = NOT_VALID;
      pu.mvpIdx[REF_PIC_LIST_0] = NOT_VALID;
      pu.mvpIdx[REF_PIC_LIST_1] = NOT_VALID;
      pu.mvpNum[REF_PIC_LIST_0] = NOT_VALID;
      pu.mvpNum[REF_PIC_LIST_1] = NOT_VALID;

#if GDR_ENABLED
    if (isEncodeGdrClean)
    {
      pu.mvSolid[REF_PIC_LIST_0] = true;
      pu.mvSolid[REF_PIC_LIST_1] = true;
      pu.mvValid[REF_PIC_LIST_0] = true;
      pu.mvValid[REF_PIC_LIST_1] = true;
    }
#endif
      // Set Motion Field

      cMv[1]     = mvValidList1;
#if GDR_ENABLED
      if (isEncodeGdrClean)
      {
        cMvSolid[1] = mvValidList1Solid;
        cMvValid[1] = mvValidList1Valid;
      }
#endif
      refIdx[1]  = refIdxValidList1;
      bits[1]    = bitsValidList1;
      uiCost[1]  = costValidList1;
#if GDR_ENABLED
      if (isEncodeGdrClean)
      {
        uiCostOk[1] = costValidList1Ok;
      }
#endif
      if (cu.cs->pps->getWPBiPred() == true && tryBipred && (bcwIdx != BCW_DEFAULT))
      {
        CHECK(iRefIdxBi[0] < 0, "Invalid picture reference index");
        CHECK(iRefIdxBi[1] < 0, "Invalid picture reference index");
        wp0 = cu.cs->slice->getWpScaling(REF_PIC_LIST_0, iRefIdxBi[0]);
        wp1 = cu.cs->slice->getWpScaling(REF_PIC_LIST_1, iRefIdxBi[1]);
        if (WPScalingParam::isWeighted(wp0) || WPScalingParam::isWeighted(wp1))
        {
          uiCostBi       = MAX_UINT;
#if GDR_ENABLED
          if (isEncodeGdrClean)
          {
            uiCostBiOk = false;
          }
#endif
          enforceBcwPred = false;
        }
      }
      if (enforceBcwPred)
      {
        uiCost[0] = uiCost[1] = MAX_UINT;
#if GDR_ENABLED
        uiCostOk[0] = uiCostOk[1] = false;
#endif
      }

      uiLastModeTemp = uiLastMode;
#if GDR_ENABLED
      allOk = ((uiCostBi <= uiCost[0] && uiCostBi <= uiCost[1]) || enforceBcwPred);

      if (isEncodeGdrClean)
      {
        if (uiCostBiOk)
        {
          allOk = (uiCostOk[0] && uiCostOk[1]) ? ((uiCostBi <= uiCost[0] && uiCostBi <= uiCost[1]) || enforceBcwPred) : true;
        }
        else
        {
          allOk = false;
        }
      }

      bool L0ok = (uiCost[0] <= uiCost[1]);

      if (isEncodeGdrClean)
      {
        if (uiCostOk[0])
        {
          L0ok = (uiCostOk[1]) ? (uiCost[0] <= uiCost[1]) : true;
        }
        else
        {
          L0ok = false;
        }
      }
#endif

#if GDR_ENABLED
      if (allOk)
#else
      if ( uiCostBi <= uiCost[0] && uiCostBi <= uiCost[1])
#endif
      {
        uiLastMode = 2;
        pu.mv    [REF_PIC_LIST_0] = cMvBi[0];
        pu.mv    [REF_PIC_LIST_1] = cMvBi[1];
        pu.mvd   [REF_PIC_LIST_0] = cMvBi[0] - cMvPredBi[0][iRefIdxBi[0]];
        pu.mvd   [REF_PIC_LIST_1] = cMvBi[1] - cMvPredBi[1][iRefIdxBi[1]];
        pu.refIdx[REF_PIC_LIST_0] = iRefIdxBi[0];
        pu.refIdx[REF_PIC_LIST_1] = iRefIdxBi[1];
        pu.mvpIdx[REF_PIC_LIST_0] = aaiMvpIdxBi[0][iRefIdxBi[0]];
        pu.mvpIdx[REF_PIC_LIST_1] = aaiMvpIdxBi[1][iRefIdxBi[1]];
        pu.mvpNum[REF_PIC_LIST_0] = aaiMvpNum[0][iRefIdxBi[0]];
        pu.mvpNum[REF_PIC_LIST_1] = aaiMvpNum[1][iRefIdxBi[1]];
        pu.interDir = 3;

        pu.cu->smvdMode = symMode;
#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          int mvp_idx0 = pu.mvpIdx[REF_PIC_LIST_0];
          int mvp_idx1 = pu.mvpIdx[REF_PIC_LIST_1];
          pu.mvSolid[REF_PIC_LIST_0] = cMvBiSolid[REF_PIC_LIST_0] && cMvPredBiSolid[REF_PIC_LIST_0][mvp_idx0];
          pu.mvSolid[REF_PIC_LIST_1] = cMvBiSolid[REF_PIC_LIST_1] && cMvPredBiSolid[REF_PIC_LIST_1][mvp_idx1];
          pu.mvValid[REF_PIC_LIST_0] = cs.isClean(pu.Y().bottomRight(), pu.mv[REF_PIC_LIST_0], (RefPicList)REF_PIC_LIST_0, pu.refIdx[REF_PIC_LIST_0]);
          pu.mvValid[REF_PIC_LIST_1] = cs.isClean(pu.Y().bottomRight(), pu.mv[REF_PIC_LIST_1], (RefPicList)REF_PIC_LIST_1, pu.refIdx[REF_PIC_LIST_1]);
        }
#endif
      }
#if GDR_ENABLED
      else if (L0ok)
#else
      else if ( uiCost[0] <= uiCost[1] )
#endif
      {
        uiLastMode = 0;
        pu.mv    [REF_PIC_LIST_0] = cMv[0];
        pu.mvd[REF_PIC_LIST_0]    = cMv[0] - cMvPred[0][refIdx[0]];
        pu.refIdx[REF_PIC_LIST_0] = refIdx[0];
        pu.mvpIdx[REF_PIC_LIST_0] = aaiMvpIdx[0][refIdx[0]];
        pu.mvpNum[REF_PIC_LIST_0] = aaiMvpNum[0][refIdx[0]];
        pu.interDir = 1;
#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          pu.mvSolid[REF_PIC_LIST_0] = cMvSolid[REF_PIC_LIST_0] && cMvPredSolid[0][refIdx[0]];
          pu.mvValid[REF_PIC_LIST_0] = cs.isClean(pu.Y().bottomRight(), pu.mv[REF_PIC_LIST_0], (RefPicList)REF_PIC_LIST_0, pu.refIdx[REF_PIC_LIST_0]);
        }
#endif
      }
      else
      {
        uiLastMode = 1;
        pu.mv    [REF_PIC_LIST_1] = cMv[1];
        pu.mvd[REF_PIC_LIST_1]    = cMv[1] - cMvPred[1][refIdx[1]];
        pu.refIdx[REF_PIC_LIST_1] = refIdx[1];
        pu.mvpIdx[REF_PIC_LIST_1] = aaiMvpIdx[1][refIdx[1]];
        pu.mvpNum[REF_PIC_LIST_1] = aaiMvpNum[1][refIdx[1]];
        pu.interDir = 2;
#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          pu.mvSolid[REF_PIC_LIST_1] = cMvSolid[REF_PIC_LIST_1] && cMvPredSolid[1][refIdx[1]];
          pu.mvValid[REF_PIC_LIST_1] = cs.isClean(pu.Y().bottomRight(), pu.mv[REF_PIC_LIST_1], (RefPicList)REF_PIC_LIST_1, pu.refIdx[REF_PIC_LIST_1]);
        }
#endif
      }

      if( bcwIdx != BCW_DEFAULT )
      {
        cu.BcwIdx = BCW_DEFAULT; // Reset to default for the Non-NormalMC modes.
      }

      //RAMIRO: função para calcular e extrair features
      #if EXTRA_FEATURES
            if(blocoAffAvaliavel){
            custoMvUniL0_1 = uiCost[0];
            custoMvUniL1_1 = uiCost[1];
            }
      #endif      

      uiHevcCost = (uiCostBi <= uiCost[0] && uiCostBi <= uiCost[1])
                     ? uiCostBi
                     : ((uiCost[0] <= uiCost[1]) ? uiCost[0] : uiCost[1]);
#if GDR_ENABLED
      if (isEncodeGdrClean)
      {
        uiHevcCostOk = (uiCostBi <= uiCost[0] && uiCostBi <= uiCost[1]) ? uiCostBiOk : ((uiCost[0] <= uiCost[1]) ? uiCostOk[0] : uiCostOk[1]);
      }
#endif
    }

//RAMIRO: função para calcular e extrair features
#if EXTRA_FEATURES
    if(blocoAffAvaliavel){   
        POC = (cu.cs->slice->getPOC());
        depth = ((int) cu.depth);
        qtDepth = ((int) cu.qtDepth);
        mtDepht = ((int) cu.mtDepth);
        videoWidth= (cu.cs->picture->Y().width);
        videoHeight = (cu.cs->picture->Y().height);
        cuPosX = (cu.Y().x);
        cuPosY = (cu.Y().y);
        cuWidth = (cu.Y().width);
        cuHeight = (cu.Y().height);
        bcw_Index = ((int)bcwIdx);
        IMV = ((int)cu.imv);

        mvUniL0HorX = (cMv[0].getHor());
        mvUniL0VerY = (cMv[0].getVer());
        mvUniL1HorX = (cMv[1].getHor());
        mvUniL1VerY = (cMv[1].getVer());

        mvPredUniL0HorX = (cMvPred[0][refIdx[0]].getHor());
        mvPredUniL0VerY = (cMvPred[0][refIdx[0]].getVer());
        mvPredUniL1HorX = (cMvPred[1][refIdx[1]].getHor());
        mvPredUniL1VerY = (cMvPred[1][refIdx[1]].getVer());

        custoMvUniL0 = (uiCost[0]);
        custoMvUniL1 = (uiCost[1]);
        bitsMvUniL0 = (bits[0]);
        bitsMvUniL1 = (bits[1]);

        mvBiL0HorX = (cMvBi[0].getHor());
        mvBiL0VerY = (cMvBi[0].getVer());
        mvBiL1HorX = (cMvBi[1].getHor());
        mvBiL1VerY = (cMvBi[1].getVer());

        mvPredBiL0HorX = (cMvPredBi[0][iRefIdxBi[0]].getHor());
        mvPredBiL0VerY = (cMvPredBi[0][iRefIdxBi[0]].getVer());
        mvPredBiL1HorX = (cMvPredBi[1][iRefIdxBi[1]].getHor());
        mvPredBiL1VerY = (cMvPredBi[1][iRefIdxBi[1]].getVer());

        custoBi = (uiCostBi);   //VTM 16.2
        //custoBi = (costBi);   //VTM 22.0
        bitsMvBi = (bits[2]);

        SMVD = (pu.cu->smvdMode);
        interDir = (pu.interDir);
        //atualQP = (pu.cs->currQP[0]);
        atualQP = cu.slice->getSliceQp();

        //executaTZS = testaTZS;
        //testaTZS = 0;
        //printf("executaTZS = %d\n", executaTZS);

        if(buscaPaiVizinho){//Extrair dados pai e vizinhos 

            strIdBlocoPaiViz = to_string(cu.cs->slice->getPOC())+to_string(pu.cs->parent->area.Y().x)+to_string(pu.cs->parent->area.Y().y)+
                    to_string(pu.cs->parent->area.Y().width)+to_string(pu.cs->parent->area.Y().height);

            idBlocoPaiViz = std::stoul(strIdBlocoPaiViz,nullptr,10);

            if(mapaBlocos.count(idBlocoPaiViz)){

                dadosPaiViz = mapaBlocos[idBlocoPaiViz];   

                custoPai = dadosPaiViz.melhorCusto;
                affinePai =  dadosPaiViz.affine;
                IMV_Pai =    dadosPaiViz.IMV;
                affTypePai = dadosPaiViz.affineType;
            }else{
                custoPai = MAX_UINT;
                affinePai = 0;
                IMV_Pai = MAX_INT;          
                affTypePai = 0;
            } 

            const PredictionUnit *neibPuLeft = nullptr;
            const PredictionUnit *neibPuAbove = nullptr;

            Position neibPosLeft;
            Position neibPosAbove;

            Position posLT = pu.Y().topLeft();
            Position posRT = pu.Y().topRight();
            Position posLB = pu.Y().bottomLeft();

            neibPosLeft = posLB.offset(-1, 0);
            neibPuLeft = cs.getPURestricted( neibPosLeft, pu, pu.chType );

            neibPosAbove = posRT.offset( 0,  -1 );
            neibPuAbove = cs.getPURestricted( neibPosAbove, pu, pu.chType );


            if (!(neibPuLeft == nullptr)){

                 strIdBlocoPaiViz = to_string(cu.cs->slice->getPOC())+to_string(neibPuLeft->Y().x)+to_string(neibPuLeft->Y().y)+
                         to_string(neibPuLeft->Y().width)+to_string(neibPuLeft->Y().height);//novo

                 idBlocoPaiViz = std::stoul(strIdBlocoPaiViz,nullptr,10);

                if(mapaBlocos.count(idBlocoPaiViz)){

                    dadosPaiViz = mapaBlocos[idBlocoPaiViz];

                    custoVizEsq = dadosPaiViz.melhorCusto;
                    affineVizEsq_1 = dadosPaiViz.affine;
                    IMV_VizEsq     = dadosPaiViz.IMV;
                    affTypeVizEsq = dadosPaiViz.affineType;

                }else{
                    custoVizEsq = MAX_UINT;
                    affineVizEsq_1 = 0;
                    IMV_VizEsq     = MAX_INT;                  
                    affTypeVizEsq = 0;
                }
                 affineVizEsq = neibPuLeft->cu->affine; // pega informação direto da PU       
            }else{
                neibPosLeft = posLB.offset(-1, 1);
                neibPuLeft = cs.getPURestricted( neibPosLeft, pu, pu.chType );
                if(!(neibPuLeft == nullptr)){
                    strIdBlocoPaiViz = to_string(cu.cs->slice->getPOC())+to_string(neibPuLeft->Y().x)+to_string(neibPuLeft->Y().y)+
                            to_string(neibPuLeft->Y().width)+to_string(neibPuLeft->Y().height);//novo

                    idBlocoPaiViz = std::stoul(strIdBlocoPaiViz,nullptr,10);

                    if(mapaBlocos.count(idBlocoPaiViz)){

                        dadosPaiViz = mapaBlocos[idBlocoPaiViz];

                        custoVizEsq = dadosPaiViz.melhorCusto;
                        affineVizEsq_1 = dadosPaiViz.affine;
                        IMV_VizEsq     = dadosPaiViz.IMV;
                        affTypeVizEsq = dadosPaiViz.affineType;
                    }else{
                        custoVizEsq = MAX_UINT;
                        affineVizEsq_1 = 0;
                        IMV_VizEsq     = MAX_INT;                  
                        affTypeVizEsq = 0;
                    }
                   affineVizEsq = neibPuLeft->cu->affine; // pega informação direto da PU
                }else{
                    affineVizEsq = 0;
                    custoVizEsq = MAX_UINT;
                    affineVizEsq_1 = 0;
                    IMV_VizEsq     = MAX_INT;                  
                    affTypeVizEsq = 0;
                }
            }

            if(!(neibPuAbove == nullptr)){
                strIdBlocoPaiViz = to_string(cu.cs->slice->getPOC())+to_string(neibPuAbove->Y().x)+to_string(neibPuAbove->Y().y)+
                        to_string(neibPuAbove->Y().width)+to_string(neibPuAbove->Y().height);//novo            

                idBlocoPaiViz = std::stoul(strIdBlocoPaiViz,nullptr,10);

                if(mapaBlocos.count(idBlocoPaiViz)){     

                    dadosPaiViz = mapaBlocos[idBlocoPaiViz];

                    custoVizAci = dadosPaiViz.melhorCusto;
                    affineVizAci_1 = dadosPaiViz.affine;
                    IMV_VizAci     = dadosPaiViz.IMV;
                    affTypeVizAci = dadosPaiViz.affineType;
                }else{
                    custoVizAci = MAX_UINT;
                    affineVizAci_1 = 0;
                    IMV_VizAci     = MAX_INT;    
                    affTypeVizAci = 0;
                }
                affineVizAci = neibPuAbove->cu->affine;// pega informação direto da PU
            }else{
                neibPosAbove = posRT.offset( 1,  -1 );
                neibPuAbove = cs.getPURestricted( neibPosAbove, pu, pu.chType );

                if(!(neibPuAbove == nullptr)){
                    strIdBlocoPaiViz = to_string(cu.cs->slice->getPOC())+to_string(neibPuAbove->Y().x)+to_string(neibPuAbove->Y().y)+
                            to_string(neibPuAbove->Y().width)+to_string(neibPuAbove->Y().height);//novo            

                    idBlocoPaiViz = std::stoul(strIdBlocoPaiViz,nullptr,10);

                    if(mapaBlocos.count(idBlocoPaiViz)){            

                        dadosPaiViz = mapaBlocos[idBlocoPaiViz];

                        custoVizAci = dadosPaiViz.melhorCusto;
                        affineVizAci_1 = dadosPaiViz.affine;
                        IMV_VizAci     = dadosPaiViz.IMV;
                        affTypeVizAci = dadosPaiViz.affineType;
                    }else{
                        custoVizAci = MAX_UINT;
                        affineVizAci_1 = 0;
                        IMV_VizAci     = MAX_INT;    
                        affTypeVizAci = 0;
                    }
                    affineVizAci = neibPuAbove->cu->affine;
                }else{       
                    neibPosAbove = posLT.offset( -1,  -1 );
                    neibPuAbove = cs.getPURestricted( neibPosAbove, pu, pu.chType );

                    if(!(neibPuAbove == nullptr)){
                                strIdBlocoPaiViz = to_string(cu.cs->slice->getPOC())+to_string(neibPuAbove->Y().x)+to_string(neibPuAbove->Y().y)+
                                        to_string(neibPuAbove->Y().width)+to_string(neibPuAbove->Y().height);//novo            

                                idBlocoPaiViz = std::stoul(strIdBlocoPaiViz,nullptr,10);

                                if(mapaBlocos.count(idBlocoPaiViz)){            

                                    dadosPaiViz = mapaBlocos[idBlocoPaiViz];

                                    custoVizAci = dadosPaiViz.melhorCusto;
                                    affineVizAci_1 = dadosPaiViz.affine;
                                    IMV_VizAci     = dadosPaiViz.IMV;
                                    affTypeVizAci = dadosPaiViz.affineType;        
                                }else{
                                    custoVizAci = MAX_UINT;
                                    affineVizAci_1 = 0;
                                    IMV_VizAci     = MAX_INT;    
                                    affTypeVizAci = 0;
                                }
                                affineVizAci = neibPuAbove->cu->affine;// pega informação direto da PU
                    }else{
                        affineVizAci = 0;
                        custoVizAci = MAX_UINT;
                        affineVizAci_1 = 0;
                        IMV_VizAci     = MAX_INT;    
                        affTypeVizAci = 0;     
                    }
                }
            }
            buscaPaiVizinho = false;
        }//fim if busca pai e vizinho      
    }//fim if blocoAffAvaliavel           
#endif    

//RAMIRO: transforma a variavel tamanho CU para entrar no XTZSearch
CUW = cu.Y().width;
CUH = cu.Y().height;

//RAMIRO: define o vetor features
if(cu.Y().width == 128 && cu.Y().height == 128){
    //features_TZS[0] = QP;
    features_TZS[1] = ((int) cu.depth);
    features_TZS[2] = ((int) cu.qtDepth);
    features_TZS[3] = ((int) cu.mtDepth);
    //features_TZS[4] = (cu.cs->picture->Y().width);
    //features_TZS[5] = (cu.cs->picture->Y().height);
    features_TZS[4] = cu.Y().x;//cuPosX;
    features_TZS[5] = cu.Y().y;//cuPosY;
    //features_TZS[8] = (int)bcwIdx;//bcwIndex;
    features_TZS[6] = int(cu.imv);
    features_TZS[7] = cMv[0].getHor();//mvUniL0HorX;
    features_TZS[8] = cMv[0].getVer();//mvUniL0VerY;
    features_TZS[9] = cMv[1].getHor();//mvUniL1HorX;
    features_TZS[10] = cMv[1].getVer();//mvUniL1VerY;
    //features_TZS[14] = uiCost[0];//custoMvUniL0;
    //features_TZS[15] = uiCost[1];//custoMvUniL1;
    features_TZS[11] = bits[0];//bitsMvUniL0;
    features_TZS[12] = bits[1];//bitsMvUniL1;
    features_TZS[13] = cMvBi[0].getHor();//mvBiL0HorX;
    features_TZS[14] = cMvBi[0].getVer();//mvBiL0VerY;
    features_TZS[15] = cMvBi[1].getHor();//mvBiL1HorX;
    features_TZS[16] = cMvBi[1].getVer();//mvBiL1VerY;
    //features_TZS[22] = (uiCostBi);//custoBi;
    features_TZS[17] = bits[2];//bitsMvBi;
    features_TZS[18] = pu.cu->smvdMode;//SMVD;
    features_TZS[19] = pu.interDir;//interDir;       
    features_TZS[21] = cu.slice->getSliceQp();//atualQP;
    features_TZS[22] = rui_SAD;
    features_TZS[23] = cStruct_iBestX;
    features_TZS[24] = cStruct_iBestY;
}else{
    //features_TZS[0] = QP;
    features_TZS[1] = ((int) cu.depth);
    features_TZS[2] = ((int) cu.qtDepth);
    features_TZS[3] = ((int) cu.mtDepth);
    //features_TZS[4] = (cu.cs->picture->Y().width);
    //features_TZS[5] = (cu.cs->picture->Y().height);
    features_TZS[4] = cu.Y().x;//cuPosX;
    features_TZS[5] = cu.Y().y;//cuPosY;
    //features_TZS[8] = (int)bcwIdx;//bcwIndex;
    features_TZS[6] = int(cu.imv);
    features_TZS[7] = cMv[0].getHor();//mvUniL0HorX;
    features_TZS[8] = cMv[0].getVer();//mvUniL0VerY;
    features_TZS[9] = cMv[1].getHor();//mvUniL1HorX;
    features_TZS[10] = cMv[1].getVer();//mvUniL1VerY;
    //features_TZS[14] = uiCost[0];//custoMvUniL0;
    //features_TZS[15] = uiCost[1];//custoMvUniL1;
    features_TZS[11] = bits[0];//bitsMvUniL0;
    features_TZS[12] = bits[1];//bitsMvUniL1;
    features_TZS[13] = cMvBi[0].getHor();//mvBiL0HorX;
    features_TZS[14] = cMvBi[0].getVer();//mvBiL0VerY;
    features_TZS[15] = cMvBi[1].getHor();//mvBiL1HorX;
    features_TZS[16] = cMvBi[1].getVer();//mvBiL1VerY;
    //features_TZS[22] = (uiCostBi);//custoBi;
    features_TZS[17] = bits[2];//bitsMvBi;
    features_TZS[18] = pu.cu->smvdMode;//SMVD;
    features_TZS[19] = pu.interDir;//interDir;       
    features_TZS[21] = cu.slice->getSliceQp();//atualQP;
    features_TZS[22] = rui_SAD;
    features_TZS[23] = cStruct_iBestX;
    features_TZS[24] = cStruct_iBestY;            
}

//RAMIRO: Ferramenta Affine
    if (cu.Y().width > 8 && cu.Y().height > 8 && cu.slice->getSPS()->getUseAffine()
      && checkAffine
      && m_pcEncCfg->getUseAffineAmvp()
      && (bcwIdx == BCW_DEFAULT || m_affineModeSelected || !m_pcEncCfg->getUseBcwFast())
      )
    {

//RAMIRO: contador de repeticoes da Affine
n_Affine++;

//RAMIRO: inicio do timer da Affine
clock_t iniClock_Affine, fimClock_Affine;
iniClock_Affine = clock();

      m_hevcCost = uiHevcCost;
#if GDR_ENABLED
      if (isEncodeGdrClean)
      {
        m_hevcCostOk = uiHevcCostOk;
      }
#endif
      // save normal hevc result
      uint32_t uiMRGIndex = pu.mergeIdx;
      bool bMergeFlag = pu.mergeFlag;
      uint32_t uiInterDir = pu.interDir;
      int  iSymMode = cu.smvdMode;

      Mv cMvd[2];
      uint32_t uiMvpIdx[2], uiMvpNum[2];
      uiMvpIdx[0] = pu.mvpIdx[REF_PIC_LIST_0];
      uiMvpIdx[1] = pu.mvpIdx[REF_PIC_LIST_1];
      uiMvpNum[0] = pu.mvpNum[REF_PIC_LIST_0];
      uiMvpNum[1] = pu.mvpNum[REF_PIC_LIST_1];
      cMvd[0]     = pu.mvd[REF_PIC_LIST_0];
      cMvd[1]     = pu.mvd[REF_PIC_LIST_1];

      MvField cHevcMvField[2];
      cHevcMvField[0].setMvField( pu.mv[REF_PIC_LIST_0], pu.refIdx[REF_PIC_LIST_0] );
      cHevcMvField[1].setMvField( pu.mv[REF_PIC_LIST_1], pu.refIdx[REF_PIC_LIST_1] );

#if GDR_ENABLED
      bool cHevcMvFieldSolid[2] = { true, true };
      bool cHevcMvFieldValid[2] = { true, true };

      if (isEncodeGdrClean)
      {
        cHevcMvFieldSolid[0] = pu.mvSolid[0];
        cHevcMvFieldSolid[1] = pu.mvSolid[1];
        cHevcMvFieldValid[0] = pu.mvValid[0];
        cHevcMvFieldValid[1] = pu.mvValid[1];
      }
#endif

      // do affine ME & Merge
      cu.affineType = AFFINEMODEL_4PARAM;
      Mv acMvAffine4Para[2][33][3];
#if GDR_ENABLED
      bool acMvAffine4ParaSolid[2][33][3];

      for (int i = 0; i < 2; i++)
        for (int j = 0; j < 33; j++)
          for (int k = 0; k < 3; k++)
            acMvAffine4ParaSolid[i][j][k] = true;
#endif
      int refIdx4Para[2] = { -1, -1 };

//RAMIRO: contador de repeticoes da Affine 4 Parametros
n_Affine_4par++;

//RAMIRO: inicio do timer da Affine 4 Parametros
clock_t iniClock_Affine_4par, fimClock_Affine_4par;
iniClock_Affine_4par = clock();

//RAMIRO: chama a funcao PredAffineInterSearch
#if GDR_ENABLED
      xPredAffineInterSearch(pu, origBuf, puIdx, uiLastModeTemp, uiAffineCost, cMvHevcTemp, cMvHevcTempSolid, acMvAffine4Para, acMvAffine4ParaSolid, refIdx4Para, bcwIdx, enforceBcwPred,
        ((cu.slice->getSPS()->getUseBcw() == true) ? getWeightIdxBits(bcwIdx) : 0));
#else
      xPredAffineInterSearch(pu, origBuf, puIdx, uiLastModeTemp, uiAffineCost, cMvHevcTemp, acMvAffine4Para, refIdx4Para, bcwIdx, enforceBcwPred,
        ((cu.slice->getSPS()->getUseBcw() == true) ? getWeightIdxBits(bcwIdx) : 0));
#endif

//RAMIRO: fim do timer da Affine 4 Parametros
fimClock_Affine_4par = clock();
totalClockAffine_4par += (fimClock_Affine_4par - iniClock_Affine_4par);

      if ( pu.cu->imv == 0 )
      {
#if GDR_ENABLED
        storeAffineMotion(pu.mvAffi, pu.mvAffiSolid, pu.refIdx, AFFINEMODEL_4PARAM, bcwIdx);
#else
        storeAffineMotion( pu.mvAffi, pu.refIdx, AFFINEMODEL_4PARAM, bcwIdx );
#endif
      }

#if GDR_ENABLED
      if (isEncodeGdrClean)
      {
        uiAffineCostOk = true;

        if (pu.interDir & 0x01)
        {
          uiAffineCostOk = uiAffineCostOk && pu.mvAffiSolid[0][0] && pu.mvAffiSolid[0][1];
          uiAffineCostOk = uiAffineCostOk && pu.mvAffiValid[0][0] && pu.mvAffiValid[0][1];
        }

        if (pu.interDir & 0x02)
        {
          uiAffineCostOk = uiAffineCostOk && pu.mvAffiSolid[1][0] && pu.mvAffiSolid[1][1];
          uiAffineCostOk = uiAffineCostOk && pu.mvAffiValid[1][0] && pu.mvAffiValid[1][1];
        }
      }
#endif

      if ( cu.slice->getSPS()->getUseAffineType() )
      {
#if GDR_ENABLED
        allOk = (uiAffineCost < uiHevcCost * 1.05);
        if (isEncodeGdrClean)
        {
          if (uiAffineCostOk)
          {
            allOk = (uiHevcCostOk) ? (uiAffineCost < uiHevcCost * 1.05) : true;
          }
          else
          {
            allOk = false;
          }
        }
#endif

#if GDR_ENABLED
        if (allOk)
#else

        //RAMIRO: Affine 6 Parametros
        if ( uiAffineCost < uiHevcCost * 1.05 ) ///< condition for 6 parameter affine ME
#endif
        {

//RAMIRO: contador de repetições da Affine 6 Parametros
n_Affine_6par++;

//RAMIRO: inicio do timer da Affine 6 Parametros
clock_t iniClock_Affine_6par, fimClock_Affine_6par;
iniClock_Affine_6par = clock();

          // save 4 parameter results
          Mv bestMv[2][3], bestMvd[2][3];
          int bestMvpIdx[2], bestMvpNum[2], bestRefIdx[2];
          uint8_t bestInterDir;

#if GDR_ENABLED
          bool bestMvSolid[2][3];
          bool bestMvValid[2][3];
#endif

          bestInterDir = pu.interDir;
          bestRefIdx[0] = pu.refIdx[0];
          bestRefIdx[1] = pu.refIdx[1];
          bestMvpIdx[0] = pu.mvpIdx[0];
          bestMvpIdx[1] = pu.mvpIdx[1];
          bestMvpNum[0] = pu.mvpNum[0];
          bestMvpNum[1] = pu.mvpNum[1];

          for ( int refList = 0; refList < 2; refList++ )
          {
            bestMv[refList][0] = pu.mvAffi[refList][0];
            bestMv[refList][1] = pu.mvAffi[refList][1];
            bestMv[refList][2] = pu.mvAffi[refList][2];
            bestMvd[refList][0] = pu.mvdAffi[refList][0];
            bestMvd[refList][1] = pu.mvdAffi[refList][1];
            bestMvd[refList][2] = pu.mvdAffi[refList][2];

#if GDR_ENABLED
            if (isEncodeGdrClean)
            {
              bestMvSolid[refList][0] = pu.mvAffiSolid[refList][0];
              bestMvSolid[refList][1] = pu.mvAffiSolid[refList][1];
              bestMvSolid[refList][2] = pu.mvAffiSolid[refList][2];

              bestMvValid[refList][0] = pu.mvAffiValid[refList][0];
              bestMvValid[refList][1] = pu.mvAffiValid[refList][1];
              bestMvValid[refList][2] = pu.mvAffiValid[refList][2];
            }
#endif
          }

          refIdx4Para[0] = bestRefIdx[0];
          refIdx4Para[1] = bestRefIdx[1];

          Distortion uiAffine6Cost = std::numeric_limits<Distortion>::max();
          cu.affineType = AFFINEMODEL_6PARAM;

//RAMIRO: chama a funcao PredAffineInterSearch
#if GDR_ENABLED
          xPredAffineInterSearch(pu, origBuf, puIdx, uiLastModeTemp, uiAffine6Cost, cMvHevcTemp, cMvHevcTempSolid, acMvAffine4Para, acMvAffine4ParaSolid, refIdx4Para, bcwIdx, enforceBcwPred,
            ((cu.slice->getSPS()->getUseBcw() == true) ? getWeightIdxBits(bcwIdx) : 0));
#else
          xPredAffineInterSearch(pu, origBuf, puIdx, uiLastModeTemp, uiAffine6Cost, cMvHevcTemp, acMvAffine4Para, refIdx4Para, bcwIdx, enforceBcwPred,
            ((cu.slice->getSPS()->getUseBcw() == true) ? getWeightIdxBits(bcwIdx) : 0));
#endif

          if ( pu.cu->imv == 0 )
          {
#if GDR_ENABLED
            storeAffineMotion(pu.mvAffi, pu.mvAffiSolid, pu.refIdx, AFFINEMODEL_6PARAM, bcwIdx);
#else
            storeAffineMotion( pu.mvAffi, pu.refIdx, AFFINEMODEL_6PARAM, bcwIdx );
#endif
          }

#if GDR_ENABLED
          if (isEncodeGdrClean)
          {
            uiAffine6CostOk = true;

            if (pu.interDir & 0x01)
            {
              uiAffine6CostOk = uiAffine6CostOk && pu.mvAffiSolid[0][0] && pu.mvAffiSolid[0][1] && pu.mvAffiSolid[0][2];
              uiAffine6CostOk = uiAffine6CostOk && pu.mvAffiValid[0][0] && pu.mvAffiValid[0][1] && pu.mvAffiValid[0][2];
            }

            if (pu.interDir & 0x02)
            {
              uiAffine6CostOk = uiAffine6CostOk && pu.mvAffiSolid[1][0] && pu.mvAffiSolid[1][1] && pu.mvAffiSolid[1][2];
              uiAffine6CostOk = uiAffine6CostOk && pu.mvAffiValid[1][0] && pu.mvAffiValid[1][1] && pu.mvAffiValid[1][2];
            }
          }
#endif

#if GDR_ENABLED
          allOk = (uiAffineCost <= uiAffine6Cost);
          if (isEncodeGdrClean)
          {
            if (uiAffineCostOk)
            {
              allOk = (uiAffine6CostOk) ? (uiAffineCost < uiHevcCost * 1.05) : true;
            }
            else
            {
              allOk = false;
            }
          }
#endif

          // reset to 4 parameter affine inter mode
#if GDR_ENABLED
          if (allOk && (uiAffineCost <= uiAffine6Cost))
#else
          if ( uiAffineCost <= uiAffine6Cost )
#endif
          {
            cu.affineType = AFFINEMODEL_4PARAM;
            pu.interDir = bestInterDir;
            pu.refIdx[0] = bestRefIdx[0];
            pu.refIdx[1] = bestRefIdx[1];
            pu.mvpIdx[0] = bestMvpIdx[0];
            pu.mvpIdx[1] = bestMvpIdx[1];
            pu.mvpNum[0] = bestMvpNum[0];
            pu.mvpNum[1] = bestMvpNum[1];

            for ( int verIdx = 0; verIdx < 3; verIdx++ )
            {
              pu.mvdAffi[REF_PIC_LIST_0][verIdx] = bestMvd[0][verIdx];
              pu.mvdAffi[REF_PIC_LIST_1][verIdx] = bestMvd[1][verIdx];
            }

            PU::setAllAffineMv( pu, bestMv[0][0], bestMv[0][1], bestMv[0][2], REF_PIC_LIST_0);
            PU::setAllAffineMv( pu, bestMv[1][0], bestMv[1][1], bestMv[1][2], REF_PIC_LIST_1);

#if GDR_ENABLED
            if (isEncodeGdrClean)
            {
              pu.mvAffiSolid[REF_PIC_LIST_0][0] = bestMvSolid[REF_PIC_LIST_0][0];
              pu.mvAffiSolid[REF_PIC_LIST_0][1] = bestMvSolid[REF_PIC_LIST_0][1];
              pu.mvAffiSolid[REF_PIC_LIST_0][2] = bestMvSolid[REF_PIC_LIST_0][2];

              pu.mvAffiValid[REF_PIC_LIST_0][0] = bestMvValid[REF_PIC_LIST_0][0];
              pu.mvAffiValid[REF_PIC_LIST_0][1] = bestMvValid[REF_PIC_LIST_0][1];
              pu.mvAffiValid[REF_PIC_LIST_0][2] = bestMvValid[REF_PIC_LIST_0][2];

              pu.mvAffiSolid[REF_PIC_LIST_1][0] = bestMvSolid[REF_PIC_LIST_1][0];
              pu.mvAffiSolid[REF_PIC_LIST_1][1] = bestMvSolid[REF_PIC_LIST_1][1];
              pu.mvAffiSolid[REF_PIC_LIST_1][2] = bestMvSolid[REF_PIC_LIST_1][2];

              pu.mvAffiValid[REF_PIC_LIST_1][0] = bestMvValid[REF_PIC_LIST_1][0];
              pu.mvAffiValid[REF_PIC_LIST_1][1] = bestMvValid[REF_PIC_LIST_1][1];
              pu.mvAffiValid[REF_PIC_LIST_1][2] = bestMvValid[REF_PIC_LIST_1][2];
            }
#endif
          }
          else
          {
            uiAffineCost = uiAffine6Cost;
#if GDR_ENABLED
            if (isEncodeGdrClean)
            {
              uiAffineCostOk = uiAffine6CostOk;
            }
#endif
          }

//RAMIRO: fim do timer da Affine 6 Parametros
fimClock_Affine_6par = clock();
totalClockAffine_6par += (fimClock_Affine_6par - iniClock_Affine_6par);

        }

        uiAffineCost += m_pcRdCost->getCost( 1 ); // add one bit for affine_type
      }

      if( uiAffineCost < uiHevcCost )
      {
        if( m_pcEncCfg->getMCTSEncConstraint() && !MCTSHelper::checkMvBufferForMCTSConstraint( pu ) )
        {
          uiAffineCost = std::numeric_limits<Distortion>::max();
        }
      }
#if GDR_ENABLED
      allOk = (uiHevcCost <= uiAffineCost);
      if (isEncodeGdrClean)
      {
        if (uiHevcCostOk)
        {
          allOk = (uiAffineCostOk) ? (uiHevcCost <= uiAffineCost) : true;
        }
        else
        {
          allOk = false;
        }
      }
#endif

#if GDR_ENABLED
      if (allOk)
#else
      if ( uiHevcCost <= uiAffineCost )
#endif
      {
        // set hevc me result
        cu.affine = false;
        pu.mergeFlag = bMergeFlag;
        pu.regularMergeFlag = false;
        pu.mergeIdx = uiMRGIndex;
        pu.interDir = uiInterDir;
        cu.smvdMode = iSymMode;
        pu.mv    [REF_PIC_LIST_0] = cHevcMvField[0].mv;
        pu.refIdx[REF_PIC_LIST_0] = cHevcMvField[0].refIdx;
        pu.mv    [REF_PIC_LIST_1] = cHevcMvField[1].mv;
        pu.refIdx[REF_PIC_LIST_1] = cHevcMvField[1].refIdx;
        pu.mvpIdx[REF_PIC_LIST_0] = uiMvpIdx[0];
        pu.mvpIdx[REF_PIC_LIST_1] = uiMvpIdx[1];
        pu.mvpNum[REF_PIC_LIST_0] = uiMvpNum[0];
        pu.mvpNum[REF_PIC_LIST_1] = uiMvpNum[1];
        pu.mvd[REF_PIC_LIST_0] = cMvd[0];
        pu.mvd[REF_PIC_LIST_1] = cMvd[1];
#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          pu.mvSolid[REF_PIC_LIST_0] = cHevcMvFieldSolid[0];
          pu.mvSolid[REF_PIC_LIST_1] = cHevcMvFieldSolid[1];
          pu.mvValid[REF_PIC_LIST_0] = cHevcMvFieldValid[0];
          pu.mvValid[REF_PIC_LIST_1] = cHevcMvFieldValid[1];
        }
#endif
      }
      else
      {

        //RAMIRO: função para calcular e extrair features
        #if EXTRA_FEATURES
                melhorCustoAtual = uiAffineCost;
        #endif

        cu.smvdMode = 0;
        CHECK( !cu.affine, "Wrong." );
        uiLastMode = uiLastModeTemp;
      }

//RAMIRO: fim do timer da Affine
fimClock_Affine = clock();
totalClockAffine += (fimClock_Affine - iniClock_Affine);

    }

    if( cu.firstPU->interDir == 3 && !cu.firstPU->mergeFlag )
    {
      if (bcwIdx != BCW_DEFAULT)
      {
        cu.BcwIdx = bcwIdx;
      }
    }
    m_maxCompIDToPred = MAX_NUM_COMPONENT;

    PU::spanMotionInfo(pu, mergeCtx);

    m_skipPROF = false;
    m_encOnly = false;
    //  MC
    PelUnitBuf predBuf = pu.cs->getPredBuf(pu);
    if ( bcwIdx == BCW_DEFAULT || !m_affineMotion.affine4ParaAvail || !m_affineMotion.affine6ParaAvail )
    {
      if (pu.cu->imv < 3)
      {
        m_affineMotion.hevcCost[pu.cu->imv] = uiHevcCost;
      }
    }
    motionCompensation( pu, predBuf, REF_PIC_LIST_X );
    puIdx++;

//RAMIRO: função para calcular e extrair features
#if EXTRA_FEATURES
    
    if(blocoAffAvaliavel){       
    
    cuAtualAffine = cu.affine;
    //cuAtualAffineType = pu.cu->affineType;
    cuAtualAffineType = 0;

    POCAtual = cu.cs->slice->getPOC();
    
    if(POCAtual != POCTemp){
         mapaBlocos.clear();
         POCTemp = POCAtual;
     }

    #if EXTRA_DADOS_MOD_1   

        if (blocosIguais){

            if(melhorCustoAtual < custoTemp){
                custoTemp = melhorCustoAtual;
                atualizaMapa = true;
            }

            if(atualizaMapa){
                db.affine = cuAtualAffine;
                db.IMV = IMV;
                db.melhorCusto = melhorCustoAtual;
                db.affineType = cuAtualAffineType;

                mapaBlocos[idBlocoAtual] = db;

                atualizaMapa = false;

                extracaoDados::setPOC(POC);
                extracaoDados::setDepth(depth);
                extracaoDados::setQtDepth(qtDepth);
                extracaoDados::setMtDepht(mtDepht);
                extracaoDados::setVideoWidth(videoWidth);
                extracaoDados::setVideoHeight(videoHeight); 
                extracaoDados::setCuPosX(cuPosX);
                extracaoDados::setCuPosY(cuPosY);
                extracaoDados::setCuWidth(cuWidth);
                extracaoDados::setCuHeight(cuHeight);
                extracaoDados::setBcwIndex(bcw_Index);
                extracaoDados::setIMV(IMV);
                extracaoDados::setMvUniL0HorX(mvUniL0HorX);
                extracaoDados::setMvUniL0VerY(mvUniL0VerY);
                extracaoDados::setMvUniL1HorX(mvUniL1HorX);
                extracaoDados::setMvUniL1VerY(mvUniL1VerY);
                extracaoDados::setMvPredUniL0HorX(mvPredUniL0HorX);
                extracaoDados::setMvPredUniL0VerY(mvPredUniL0VerY);
                extracaoDados::setMvPredUniL1HorX(mvPredUniL1HorX);
                extracaoDados::setMvPredUniL1VerY(mvPredUniL1VerY);
                extracaoDados::setCustoMvUniL0(custoMvUniL0);
                extracaoDados::setCustoMvUniL1(custoMvUniL1);
                extracaoDados::setCustoMvUniL0_1(custoMvUniL0_1);
                extracaoDados::setCustoMvUniL1_1(custoMvUniL1_1);
                extracaoDados::setBitsMvUniL0(bitsMvUniL0);
                extracaoDados::setBitsMvUniL1(bitsMvUniL1);
                extracaoDados::setMvBiL0HorX(mvBiL0HorX);
                extracaoDados::setMvBiL0VerY(mvBiL0VerY);
                extracaoDados::setMvBiL1HorX(mvBiL1HorX);
                extracaoDados::setMvBiL1VerY(mvBiL1VerY);
                extracaoDados::setMvPredBiL0HorX(mvPredBiL0HorX);
                extracaoDados::setMvPredBiL0VerY(mvPredBiL0VerY);
                extracaoDados::setMvPredBiL1HorX(mvPredBiL1HorX);
                extracaoDados::setMvPredBiL1VerY(mvPredBiL1VerY);
                extracaoDados::setCustoBi(custoBi);
                extracaoDados::setBitsMvBi(bitsMvBi);
                extracaoDados::setSMVD(SMVD);
                extracaoDados::setInterDir(interDir);
                extracaoDados::setAtualQP(atualQP);
                extracaoDados::setAffinePai(affinePai);
                extracaoDados::setCustoPai(custoPai);
                extracaoDados::setIMV_Pai(IMV_Pai);
                extracaoDados::setAffineVizEsq(affineVizEsq);
                extracaoDados::setAffineVizEsq_1(affineVizEsq_1);
                extracaoDados::setCustoVizEsq(custoVizEsq);
                extracaoDados::setIMV_VizEsq(IMV_VizEsq);
                extracaoDados::setAffineVizAci(affineVizAci);
                extracaoDados::setAffineVizAci_1(affineVizAci_1);
                extracaoDados::setCustoVizAci(custoVizAci);
                extracaoDados::setIMV_VizAci(IMV_VizAci);
                extracaoDados::setCuAtualAffine(cuAtualAffine);
                extracaoDados::setAffCustoMvUniL0(affCustoMvUniL0);
                extracaoDados::setAffCustoMvUniL1(affCustoMvUniL1);
                extracaoDados::setAffCustoMvUniL0_1(affCustoMvUniL0_1);
                extracaoDados::setAffCustoMvUniL1_1(affCustoMvUniL1_1);
                extracaoDados::setAffCustoMvBi(affCustoMvBi);
                extracaoDados::setAffMvUniL0P0HorX(affMvUniL0P0HorX);
                extracaoDados::setAffMvUniL0P0VerY(affMvUniL0P0VerY);
                extracaoDados::setAffMvUniL0P1HorX(affMvUniL0P1HorX);
                extracaoDados::setAffMvUniL0P1VerY(affMvUniL0P1VerY);
                extracaoDados::setAffMvUniL1P0HorX(affMvUniL1P0HorX);
                extracaoDados::setAffMvUniL1P0VerY(affMvUniL1P0VerY);
                extracaoDados::setAffMvUniL1P1HorX(affMvUniL1P1HorX);
                extracaoDados::setAffMvUniL1P1VerY(affMvUniL1P1VerY);
                extracaoDados::setAffMvPredUniL0P0HorX(affMvPredUniL0P0HorX);
                extracaoDados::setAffMvPredUniL0P0VerY(affMvPredUniL0P0VerY);
                extracaoDados::setAffMvPredUniL0P1HorX(affMvPredUniL0P1HorX);
                extracaoDados::setAffMvPredUniL0P1VerY(affMvPredUniL0P1VerY);
                extracaoDados::setAffMvPredUniL1P0HorX(affMvPredUniL1P0HorX);
                extracaoDados::setAffMvPredUniL1P0VerY(affMvPredUniL1P0VerY);
                extracaoDados::setAffMvPredUniL1P1HorX(affMvPredUniL1P1HorX);
                extracaoDados::setAffMvPredUniL1P1VerY(affMvPredUniL1P1VerY);
                extracaoDados::setAffMvBiL0P0HorX(affMvBiL0P0HorX);
                extracaoDados::setAffMvBiL0P0VerY(affMvBiL0P0VerY);
                extracaoDados::setAffMvBiL0P1HorX(affMvBiL0P1HorX);
                extracaoDados::setAffMvBiL0P1VerY(affMvBiL0P1VerY);
                extracaoDados::setAffMvBiL1P0HorX(affMvBiL1P0HorX);
                extracaoDados::setAffMvBiL1P0VerY(affMvBiL1P0VerY);
                extracaoDados::setAffMvBiL1P1HorX(affMvBiL1P1HorX);
                extracaoDados::setAffMvBiL1P1VerY(affMvBiL1P1VerY);
                extracaoDados::setAffMvPredBiL0P0HorX(affMvPredBiL0P0HorX);
                extracaoDados::setAffMvPredBiL0P0VerY(affMvPredBiL0P0VerY);
                extracaoDados::setAffMvPredBiL0P1HorX(affMvPredBiL0P1HorX);
                extracaoDados::setAffMvPredBiL0P1VerY(affMvPredBiL0P1VerY);
                extracaoDados::setAffMvPredBiL1P0HorX(affMvPredBiL1P0HorX);
                extracaoDados::setAffMvPredBiL1P0VerY(affMvPredBiL1P0VerY);
                extracaoDados::setAffMvPredBiL1P1HorX(affMvPredBiL1P1HorX);
                extracaoDados::setaffMvPredBiL1P1VerY(affMvPredBiL1P1VerY);
                extracaoDados::setAffbitsMvUniL0(affbitsMvUniL0);
                extracaoDados::setAffBitsMvUniL1(affBitsMvUniL1);
                extracaoDados::setAffBitsMvBi(affBitsMvBi);
                extracaoDados::setAffInterDir(affInterDir);
                extracaoDados::setAffTypePai(affTypePai);
                extracaoDados::setAffTypeVizEsq(affTypeVizEsq);
                extracaoDados::setAffTypeVizAci(affTypeVizAci);
                extracaoDados::setCuAtualAffineType(cuAtualAffineType);
                extracaoDados::setrui_SAD(rui_SAD);
                extracaoDados::setcStruct_iBestX(cStruct_iBestX);
                extracaoDados::setcStruct_iBestY(cStruct_iBestY);
                extracaoDados::setexecutaTZS(executaTZS);

                atualizaMapa = false;
            }

        }else{
            custoTemp = melhorCustoAtual;
            idBlocoTemp = idBlocoAtual;

            db.affine = cuAtualAffine;
            db.IMV = IMV;
            db.melhorCusto = melhorCustoAtual;
            db.affineType = cuAtualAffineType;

            mapaBlocos[idBlocoAtual] = db;

            extracaoDados::setPOC(POC);
            extracaoDados::setDepth(depth);
            extracaoDados::setQtDepth(qtDepth);
            extracaoDados::setMtDepht(mtDepht);
            extracaoDados::setVideoWidth(videoWidth);
            extracaoDados::setVideoHeight(videoHeight); 
            extracaoDados::setCuPosX(cuPosX);
            extracaoDados::setCuPosY(cuPosY);
            extracaoDados::setCuWidth(cuWidth);
            extracaoDados::setCuHeight(cuHeight);
            extracaoDados::setBcwIndex(bcw_Index);
            extracaoDados::setIMV(IMV);
            extracaoDados::setMvUniL0HorX(mvUniL0HorX);
            extracaoDados::setMvUniL0VerY(mvUniL0VerY);
            extracaoDados::setMvUniL1HorX(mvUniL1HorX);
            extracaoDados::setMvUniL1VerY(mvUniL1VerY);
            extracaoDados::setMvPredUniL0HorX(mvPredUniL0HorX);
            extracaoDados::setMvPredUniL0VerY(mvPredUniL0VerY);
            extracaoDados::setMvPredUniL1HorX(mvPredUniL1HorX);
            extracaoDados::setMvPredUniL1VerY(mvPredUniL1VerY);
            extracaoDados::setCustoMvUniL0(custoMvUniL0);
            extracaoDados::setCustoMvUniL1(custoMvUniL1);
            extracaoDados::setCustoMvUniL0_1(custoMvUniL0_1);
            extracaoDados::setCustoMvUniL1_1(custoMvUniL1_1);
            extracaoDados::setBitsMvUniL0(bitsMvUniL0);
            extracaoDados::setBitsMvUniL1(bitsMvUniL1);
            extracaoDados::setMvBiL0HorX(mvBiL0HorX);
            extracaoDados::setMvBiL0VerY(mvBiL0VerY);
            extracaoDados::setMvBiL1HorX(mvBiL1HorX);
            extracaoDados::setMvBiL1VerY(mvBiL1VerY);
            extracaoDados::setMvPredBiL0HorX(mvPredBiL0HorX);
            extracaoDados::setMvPredBiL0VerY(mvPredBiL0VerY);
            extracaoDados::setMvPredBiL1HorX(mvPredBiL1HorX);
            extracaoDados::setMvPredBiL1VerY(mvPredBiL1VerY);
            extracaoDados::setCustoBi(custoBi);
            extracaoDados::setBitsMvBi(bitsMvBi);
            extracaoDados::setSMVD(SMVD);
            extracaoDados::setInterDir(interDir);
            extracaoDados::setAtualQP(atualQP);
            extracaoDados::setAffinePai(affinePai);
            extracaoDados::setCustoPai(custoPai);
            extracaoDados::setIMV_Pai(IMV_Pai);
            extracaoDados::setAffineVizEsq(affineVizEsq);
            extracaoDados::setAffineVizEsq_1(affineVizEsq_1);
            extracaoDados::setCustoVizEsq(custoVizEsq);
            extracaoDados::setIMV_VizEsq(IMV_VizEsq);
            extracaoDados::setAffineVizAci(affineVizAci);
            extracaoDados::setAffineVizAci_1(affineVizAci_1);
            extracaoDados::setCustoVizAci(custoVizAci);
            extracaoDados::setIMV_VizAci(IMV_VizAci);
            extracaoDados::setCuAtualAffine(cuAtualAffine);
            extracaoDados::setAffCustoMvUniL0(affCustoMvUniL0);
            extracaoDados::setAffCustoMvUniL1(affCustoMvUniL1);
            extracaoDados::setAffCustoMvUniL0_1(affCustoMvUniL0_1);
            extracaoDados::setAffCustoMvUniL1_1(affCustoMvUniL1_1);
            extracaoDados::setAffCustoMvBi(affCustoMvBi);
            extracaoDados::setAffMvUniL0P0HorX(affMvUniL0P0HorX);
            extracaoDados::setAffMvUniL0P0VerY(affMvUniL0P0VerY);
            extracaoDados::setAffMvUniL0P1HorX(affMvUniL0P1HorX);
            extracaoDados::setAffMvUniL0P1VerY(affMvUniL0P1VerY);
            extracaoDados::setAffMvUniL1P0HorX(affMvUniL1P0HorX);
            extracaoDados::setAffMvUniL1P0VerY(affMvUniL1P0VerY);
            extracaoDados::setAffMvUniL1P1HorX(affMvUniL1P1HorX);
            extracaoDados::setAffMvUniL1P1VerY(affMvUniL1P1VerY);
            extracaoDados::setAffMvPredUniL0P0HorX(affMvPredUniL0P0HorX);
            extracaoDados::setAffMvPredUniL0P0VerY(affMvPredUniL0P0VerY);
            extracaoDados::setAffMvPredUniL0P1HorX(affMvPredUniL0P1HorX);
            extracaoDados::setAffMvPredUniL0P1VerY(affMvPredUniL0P1VerY);
            extracaoDados::setAffMvPredUniL1P0HorX(affMvPredUniL1P0HorX);
            extracaoDados::setAffMvPredUniL1P0VerY(affMvPredUniL1P0VerY);
            extracaoDados::setAffMvPredUniL1P1HorX(affMvPredUniL1P1HorX);
            extracaoDados::setAffMvPredUniL1P1VerY(affMvPredUniL1P1VerY);
            extracaoDados::setAffMvBiL0P0HorX(affMvBiL0P0HorX);
            extracaoDados::setAffMvBiL0P0VerY(affMvBiL0P0VerY);
            extracaoDados::setAffMvBiL0P1HorX(affMvBiL0P1HorX);
            extracaoDados::setAffMvBiL0P1VerY(affMvBiL0P1VerY);
            extracaoDados::setAffMvBiL1P0HorX(affMvBiL1P0HorX);
            extracaoDados::setAffMvBiL1P0VerY(affMvBiL1P0VerY);
            extracaoDados::setAffMvBiL1P1HorX(affMvBiL1P1HorX);
            extracaoDados::setAffMvBiL1P1VerY(affMvBiL1P1VerY);
            extracaoDados::setAffMvPredBiL0P0HorX(affMvPredBiL0P0HorX);
            extracaoDados::setAffMvPredBiL0P0VerY(affMvPredBiL0P0VerY);
            extracaoDados::setAffMvPredBiL0P1HorX(affMvPredBiL0P1HorX);
            extracaoDados::setAffMvPredBiL0P1VerY(affMvPredBiL0P1VerY);
            extracaoDados::setAffMvPredBiL1P0HorX(affMvPredBiL1P0HorX);
            extracaoDados::setAffMvPredBiL1P0VerY(affMvPredBiL1P0VerY);
            extracaoDados::setAffMvPredBiL1P1HorX(affMvPredBiL1P1HorX);
            extracaoDados::setaffMvPredBiL1P1VerY(affMvPredBiL1P1VerY);
            extracaoDados::setAffbitsMvUniL0(affbitsMvUniL0);
            extracaoDados::setAffBitsMvUniL1(affBitsMvUniL1);
            extracaoDados::setAffBitsMvBi(affBitsMvBi);
            extracaoDados::setAffInterDir(affInterDir);
            extracaoDados::setAffTypePai(affTypePai);
            extracaoDados::setAffTypeVizEsq(affTypeVizEsq);
            extracaoDados::setAffTypeVizAci(affTypeVizAci);
            extracaoDados::setCuAtualAffineType(cuAtualAffineType);      
            extracaoDados::setrui_SAD(rui_SAD);      
            extracaoDados::setcStruct_iBestX(cStruct_iBestX);      
            extracaoDados::setcStruct_iBestY(cStruct_iBestY);    
            extracaoDados::setexecutaTZS(executaTZS);    
        }

        custoMvUniL0_1 = MAX_UINT64;
        custoMvUniL1_1 = MAX_UINT64;
        affCustoMvUniL0 = 111111;
        affCustoMvUniL1 = 111111;
        affCustoMvUniL0_1 = 111111;
        affCustoMvUniL1_1 = 111111;
        affCustoMvBi = 111111;
        affMvUniL0P0HorX = 111111;
        affMvUniL0P0VerY = 111111;
        affMvUniL0P1HorX = 111111;
        affMvUniL0P1VerY = 111111;
        affMvUniL1P0HorX = 111111;
        affMvUniL1P0VerY = 111111;
        affMvUniL1P1HorX = 111111;
        affMvUniL1P1VerY = 111111;
        affMvPredUniL0P0HorX = 111111;
        affMvPredUniL0P0VerY = 111111;
        affMvPredUniL0P1HorX = 111111;
        affMvPredUniL0P1VerY = 111111;
        affMvPredUniL1P0HorX = 111111;
        affMvPredUniL1P0VerY = 111111;
        affMvPredUniL1P1HorX = 111111;
        affMvPredUniL1P1VerY = 111111;
        affMvBiL0P0HorX = 111111;
        affMvBiL0P0VerY = 111111;
        affMvBiL0P1HorX = 111111;
        affMvBiL0P1VerY = 111111;
        affMvBiL1P0HorX = 111111;
        affMvBiL1P0VerY = 111111;
        affMvBiL1P1HorX = 111111;
        affMvBiL1P1VerY = 111111;
        affMvPredBiL0P0HorX = 111111;
        affMvPredBiL0P0VerY = 111111;
        affMvPredBiL0P1HorX = 111111;
        affMvPredBiL0P1VerY = 111111;
        affMvPredBiL1P0HorX = 111111;
        affMvPredBiL1P0VerY = 111111;
        affMvPredBiL1P1HorX = 111111;
        affMvPredBiL1P1VerY = 111111;
        affbitsMvUniL0 = 111111;
        affBitsMvUniL1 = 111111;
        affBitsMvBi = 111111;
        affInterDir = 111111;
        cuAtualAffineType = 0;     
    #endif
            
    #if EXTRA_DADOS_MOD_2

        if (blocosIguais){

            if(melhorCustoAtual < custoTemp){
                custoTemp = melhorCustoAtual;
                atualizaMapa = true;
            }

            if(atualizaMapa){
                db.affine = cuAtualAffine;
                db.IMV = IMV;
                db.melhorCusto = melhorCustoAtual;
                db.affineType = cuAtualAffineType;

                mapaBlocos[idBlocoAtual] = db;

                atualizaMapa = false;
            }

        }else{
            custoTemp = melhorCustoAtual;
            idBlocoTemp = idBlocoAtual;

            db.affine = cuAtualAffine;
            db.IMV = IMV;
            db.melhorCusto = melhorCustoAtual;
            db.affineType = cuAtualAffineType;

            mapaBlocos[idBlocoAtual] = db;  
        }

        extracaoDados::setPOC(POC);
        extracaoDados::setDepth(depth);
        extracaoDados::setQtDepth(qtDepth);
        extracaoDados::setMtDepht(mtDepht);
        extracaoDados::setVideoWidth(videoWidth);
        extracaoDados::setVideoHeight(videoHeight); 
        extracaoDados::setCuPosX(cuPosX);
        extracaoDados::setCuPosY(cuPosY);
        extracaoDados::setCuWidth(cuWidth);
        extracaoDados::setCuHeight(cuHeight);
        extracaoDados::setBcwIndex(bcw_Index);
        extracaoDados::setIMV(IMV);
        extracaoDados::setMvUniL0HorX(mvUniL0HorX);
        extracaoDados::setMvUniL0VerY(mvUniL0VerY);
        extracaoDados::setMvUniL1HorX(mvUniL1HorX);
        extracaoDados::setMvUniL1VerY(mvUniL1VerY);            
        extracaoDados::setMvPredUniL0HorX(mvPredUniL0HorX);
        extracaoDados::setMvPredUniL0VerY(mvPredUniL0VerY);
        extracaoDados::setMvPredUniL1HorX(mvPredUniL1HorX);
        extracaoDados::setMvPredUniL1VerY(mvPredUniL1VerY);            
        extracaoDados::setCustoMvUniL0(custoMvUniL0);
        extracaoDados::setCustoMvUniL1(custoMvUniL1);
        extracaoDados::setCustoMvUniL0_1(custoMvUniL0_1);
        extracaoDados::setCustoMvUniL1_1(custoMvUniL1_1);            
        extracaoDados::setBitsMvUniL0(bitsMvUniL0);
        extracaoDados::setBitsMvUniL1(bitsMvUniL1);
        extracaoDados::setMvBiL0HorX(mvBiL0HorX);
        extracaoDados::setMvBiL0VerY(mvBiL0VerY);
        extracaoDados::setMvBiL1HorX(mvBiL1HorX);
        extracaoDados::setMvBiL1VerY(mvBiL1VerY);            
        extracaoDados::setMvPredBiL0HorX(mvPredBiL0HorX);
        extracaoDados::setMvPredBiL0VerY(mvPredBiL0VerY);
        extracaoDados::setMvPredBiL1HorX(mvPredBiL1HorX);
        extracaoDados::setMvPredBiL1VerY(mvPredBiL1VerY);            
        extracaoDados::setCustoBi(custoBi);
        extracaoDados::setBitsMvBi(bitsMvBi);
        extracaoDados::setSMVD(SMVD);
        extracaoDados::setInterDir(interDir);
        extracaoDados::setAtualQP(atualQP);
        extracaoDados::setAffinePai(affinePai);
        extracaoDados::setCustoPai(custoPai);
        extracaoDados::setIMV_Pai(IMV_Pai);            
        extracaoDados::setAffineVizEsq(affineVizEsq);
        extracaoDados::setAffineVizEsq_1(affineVizEsq_1);
        extracaoDados::setCustoVizEsq(custoVizEsq);
        extracaoDados::setIMV_VizEsq(IMV_VizEsq);            
        extracaoDados::setAffineVizAci(affineVizAci);
        extracaoDados::setAffineVizAci_1(affineVizAci_1);
        extracaoDados::setCustoVizAci(custoVizAci);
        extracaoDados::setIMV_VizAci(IMV_VizAci);            
        extracaoDados::setCuAtualAffine(cuAtualAffine);            
        extracaoDados::setAffCustoMvUniL0(affCustoMvUniL0);
        extracaoDados::setAffCustoMvUniL1(affCustoMvUniL1);
        extracaoDados::setAffCustoMvUniL0_1(affCustoMvUniL0_1);
        extracaoDados::setAffCustoMvUniL1_1(affCustoMvUniL1_1);            
        extracaoDados::setAffCustoMvBi(affCustoMvBi);            
        extracaoDados::setAffMvUniL0P0HorX(affMvUniL0P0HorX);
        extracaoDados::setAffMvUniL0P0VerY(affMvUniL0P0VerY);
        extracaoDados::setAffMvUniL0P1HorX(affMvUniL0P1HorX);
        extracaoDados::setAffMvUniL0P1VerY(affMvUniL0P1VerY);            
        extracaoDados::setAffMvUniL1P0HorX(affMvUniL1P0HorX);
        extracaoDados::setAffMvUniL1P0VerY(affMvUniL1P0VerY);
        extracaoDados::setAffMvUniL1P1HorX(affMvUniL1P1HorX);
        extracaoDados::setAffMvUniL1P1VerY(affMvUniL1P1VerY);            
        extracaoDados::setAffMvPredUniL0P0HorX(affMvPredUniL0P0HorX);
        extracaoDados::setAffMvPredUniL0P0VerY(affMvPredUniL0P0VerY);
        extracaoDados::setAffMvPredUniL0P1HorX(affMvPredUniL0P1HorX);
        extracaoDados::setAffMvPredUniL0P1VerY(affMvPredUniL0P1VerY);            
        extracaoDados::setAffMvPredUniL1P0HorX(affMvPredUniL1P0HorX);
        extracaoDados::setAffMvPredUniL1P0VerY(affMvPredUniL1P0VerY);
        extracaoDados::setAffMvPredUniL1P1HorX(affMvPredUniL1P1HorX);
        extracaoDados::setAffMvPredUniL1P1VerY(affMvPredUniL1P1VerY);            
        extracaoDados::setAffMvBiL0P0HorX(affMvBiL0P0HorX);
        extracaoDados::setAffMvBiL0P0VerY(affMvBiL0P0VerY);
        extracaoDados::setAffMvBiL0P1HorX(affMvBiL0P1HorX);
        extracaoDados::setAffMvBiL0P1VerY(affMvBiL0P1VerY);    
        extracaoDados::setAffMvBiL1P0HorX(affMvBiL1P0HorX);
        extracaoDados::setAffMvBiL1P0VerY(affMvBiL1P0VerY);
        extracaoDados::setAffMvBiL1P1HorX(affMvBiL1P1HorX);
        extracaoDados::setAffMvBiL1P1VerY(affMvBiL1P1VerY);            
        extracaoDados::setAffMvPredBiL0P0HorX(affMvPredBiL0P0HorX);
        extracaoDados::setAffMvPredBiL0P0VerY(affMvPredBiL0P0VerY);
        extracaoDados::setAffMvPredBiL0P1HorX(affMvPredBiL0P1HorX);
        extracaoDados::setAffMvPredBiL0P1VerY(affMvPredBiL0P1VerY);    
        extracaoDados::setAffMvPredBiL1P0HorX(affMvPredBiL1P0HorX);
        extracaoDados::setAffMvPredBiL1P0VerY(affMvPredBiL1P0VerY);
        extracaoDados::setAffMvPredBiL1P1HorX(affMvPredBiL1P1HorX);
        extracaoDados::setaffMvPredBiL1P1VerY(affMvPredBiL1P1VerY);            
        extracaoDados::setAffbitsMvUniL0(affbitsMvUniL0);
        extracaoDados::setAffBitsMvUniL1(affBitsMvUniL1);
        extracaoDados::setAffBitsMvBi(affBitsMvBi);            
        extracaoDados::setAffInterDir(affInterDir);            
        extracaoDados::setAffTypePai(affTypePai);
        extracaoDados::setAffTypeVizEsq(affTypeVizEsq);
        extracaoDados::setAffTypeVizAci(affTypeVizAci);
        extracaoDados::setCuAtualAffineType(cuAtualAffineType);   
        extracaoDados::setrui_SAD(rui_SAD);  
        extracaoDados::setcStruct_iBestX(cStruct_iBestX);  
        extracaoDados::setcStruct_iBestY(cStruct_iBestY); 
        //extracaoDados::setexecutaTZS(executaTZS); 

        extracaoDados::registraFeatures();

        custoMvUniL0_1 = MAX_UINT64;
        custoMvUniL1_1 = MAX_UINT64;    
        affCustoMvUniL0 = 111111;
        affCustoMvUniL1 = 111111;
        affCustoMvUniL0_1 = 111111;
        affCustoMvUniL1_1 = 111111;
        affCustoMvBi = 111111;
        affMvUniL0P0HorX = 111111;
        affMvUniL0P0VerY = 111111;
        affMvUniL0P1HorX = 111111;
        affMvUniL0P1VerY = 111111;
        affMvUniL1P0HorX = 111111;
        affMvUniL1P0VerY = 111111;
        affMvUniL1P1HorX = 111111;
        affMvUniL1P1VerY = 111111;
        affMvPredUniL0P0HorX = 111111;
        affMvPredUniL0P0VerY = 111111;
        affMvPredUniL0P1HorX = 111111;
        affMvPredUniL0P1VerY = 111111;
        affMvPredUniL1P0HorX = 111111;
        affMvPredUniL1P0VerY = 111111;
        affMvPredUniL1P1HorX = 111111;
        affMvPredUniL1P1VerY = 111111;
        affMvBiL0P0HorX = 111111;
        affMvBiL0P0VerY = 111111;
        affMvBiL0P1HorX = 111111;
        affMvBiL0P1VerY = 111111;
        affMvBiL1P0HorX = 111111;
        affMvBiL1P0VerY = 111111;
        affMvBiL1P1HorX = 111111;
        affMvBiL1P1VerY = 111111;
        affMvPredBiL0P0HorX = 111111;
        affMvPredBiL0P0VerY = 111111;
        affMvPredBiL0P1HorX = 111111;
        affMvPredBiL0P1VerY = 111111;
        affMvPredBiL1P0HorX = 111111;
        affMvPredBiL1P0VerY = 111111;
        affMvPredBiL1P1HorX = 111111;
        affMvPredBiL1P1VerY = 111111;
        affbitsMvUniL0 = 111111;
        affBitsMvUniL1 = 111111;
        affBitsMvBi = 111111;
        affInterDir = 111111;
        cuAtualAffineType = 0;           
    #endif
            
}//fim if(blocoAffAvaliavel)   
    
#endif 

  }

  setWpScalingDistParam( -1, REF_PIC_LIST_X, cu.cs->slice );

  return;
}

uint32_t InterSearch::xCalcAffineMVBits( PredictionUnit& pu, Mv acMvTemp[3], Mv acMvPred[3] )
{
  int mvNum  = pu.cu->affineType ? 3 : 2;
  m_pcRdCost->setCostScale( 0 );
  uint32_t bitsTemp = 0;

  for ( int verIdx = 0; verIdx < mvNum; verIdx++ )
  {
    Mv pred = verIdx == 0 ? acMvPred[verIdx] : acMvPred[verIdx] + acMvTemp[0] - acMvPred[0];
    pred.changeAffinePrecInternal2Amvr(pu.cu->imv);
    m_pcRdCost->setPredictor( pred );
    Mv mv = acMvTemp[verIdx];
    mv.changeAffinePrecInternal2Amvr(pu.cu->imv);

    bitsTemp += m_pcRdCost->getBitsOfVectorWithPredictor( mv.getHor(), mv.getVer(), 0 );
  }

  return bitsTemp;
}

// AMVP
void InterSearch::xEstimateMvPredAMVP(PredictionUnit &pu, PelUnitBuf &origBuf, RefPicList eRefPicList, int refIdx,
                                      Mv &rcMvPred, AMVPInfo &rAMVPInfo, bool bFilled, Distortion *puiDistBiP)
{
  Mv         cBestMv;
  int        iBestIdx   = 0;
  Distortion uiBestCost = std::numeric_limits<Distortion>::max();
  int        i;

  AMVPInfo*  pcAMVPInfo = &rAMVPInfo;
#if GDR_ENABLED
  const CodingStructure &cs = *pu.cs;
  const bool isEncodeGdrClean = cs.sps->getGDREnabledFlag() && cs.pcv->isEncoder && ((cs.picHeader->getInGdrInterval() && cs.isClean(pu.Y().topRight(), CHANNEL_TYPE_LUMA)) || (cs.picHeader->getNumVerVirtualBoundaries() == 0));
#endif

#if GDR_ENABLED
  if (isEncodeGdrClean)
  {
    pcAMVPInfo->allCandSolidInAbove = true;
    for (int i = 0; i < AMVP_MAX_NUM_CANDS_MEM; i++)
    {
      pcAMVPInfo->mvSolid[i] = true;
      pcAMVPInfo->mvValid[i] = true;
    }
  }

  bool uiBestCostOk = false;
  bool uiTmpCostOk = false;
#endif

  // Fill the MV Candidates
  if (!bFilled)
  {
    PU::fillMvpCand(pu, eRefPicList, refIdx, *pcAMVPInfo);
  }

  // initialize Mvp index & Mvp
  iBestIdx = 0;
  cBestMv  = pcAMVPInfo->mvCand[0];
#if GDR_ENABLED
  if (isEncodeGdrClean)
  {
    uiBestCostOk = pcAMVPInfo->mvSolid[0];
  }
#endif

  PelUnitBuf predBuf = m_tmpStorageLCU.getBuf( UnitAreaRelative(*pu.cu, pu) );

  //-- Check Minimum Cost.
  for( i = 0 ; i < pcAMVPInfo->numCand; i++)
  {
    Distortion uiTmpCost =
      xGetTemplateCost(pu, origBuf, predBuf, pcAMVPInfo->mvCand[i], i, AMVP_MAX_NUM_CANDS, eRefPicList, refIdx);

#if GDR_ENABLED
    if (isEncodeGdrClean)
    {
      uiTmpCostOk = pcAMVPInfo->mvSolid[i];
    }
#endif

#if GDR_ENABLED
    bool allOk = (uiBestCost > uiTmpCost);

    if (isEncodeGdrClean)
    {
      if (uiBestCostOk)
      {
        allOk = (uiTmpCostOk) ? (uiBestCost > uiTmpCost) : true;
      }
      else
      {
        allOk = false;
      }
    }
#endif

#if GDR_ENABLED
    if (allOk)
#else
    if( uiBestCost > uiTmpCost )
#endif
    {
      uiBestCost     = uiTmpCost;
      cBestMv        = pcAMVPInfo->mvCand[i];
      iBestIdx       = i;
      (*puiDistBiP)  = uiTmpCost;
#if GDR_ENABLED
      if (isEncodeGdrClean)
      {
        uiBestCostOk = uiTmpCostOk;
      }
#endif
    }
  }

  // Setting Best MVP
  rcMvPred = cBestMv;
  pu.mvpIdx[eRefPicList] = iBestIdx;
  pu.mvpNum[eRefPicList] = pcAMVPInfo->numCand;

#if GDR_ENABLED
  if (isEncodeGdrClean)
  {
    pu.mvpSolid[eRefPicList] = pcAMVPInfo->mvSolid[iBestIdx];
  }
#endif

  return;
}

uint32_t InterSearch::xGetMvpIdxBits(int idx, int iNum)
{
  CHECK(idx < 0 || iNum < 0 || idx >= iNum, "Invalid parameters");

  if (iNum == 1)
  {
    return 0;
  }

  uint32_t length   = 1;
  int      temp     = idx;
  if (temp == 0)
  {
    return length;
  }

  bool bCodeLast = (iNum - 1 > temp);

  length += (temp - 1);

  if( bCodeLast )
  {
    length++;
  }

  return length;
}

void InterSearch::xGetBlkBits( bool bPSlice, int iPartIdx, uint32_t uiLastMode, uint32_t uiBlkBit[3])
{
  uiBlkBit[0] = (! bPSlice) ? 3 : 1;
  uiBlkBit[1] = 3;
  uiBlkBit[2] = 5;
}

void InterSearch::xCopyAMVPInfo (AMVPInfo* pSrc, AMVPInfo* pDst)
{
  pDst->numCand = pSrc->numCand;
  for (int i = 0; i < pSrc->numCand; i++)
  {
    pDst->mvCand[i] = pSrc->mvCand[i];
#if GDR_ENABLED
    pDst->mvPos[i] = pSrc->mvPos[i];
    pDst->mvSolid[i] = pSrc->mvSolid[i];
    pDst->mvValid[i] = pSrc->mvValid[i];
    pDst->mvType[i] = pSrc->mvType[i];
#endif
  }
}

#if GDR_ENABLED
void InterSearch::xCheckBestMVP(PredictionUnit &pu, RefPicList eRefPicList, Mv cMv, Mv& rcMvPred, int& riMVPIdx, AMVPInfo& amvpInfo, uint32_t& ruiBits, Distortion& ruiCost, const uint8_t imv)
#else
void InterSearch::xCheckBestMVP ( RefPicList eRefPicList, Mv cMv, Mv& rcMvPred, int& riMVPIdx, AMVPInfo& amvpInfo, uint32_t& ruiBits, Distortion& ruiCost, const uint8_t imv )
#endif
{
#if GDR_ENABLED
  const CodingStructure &cs = *pu.cs;
  const bool isEncodeGdrClean = cs.sps->getGDREnabledFlag() && cs.pcv->isEncoder && ((cs.picHeader->getInGdrInterval() && cs.isClean(pu.Y().topRight(), CHANNEL_TYPE_LUMA)) || (cs.picHeader->getNumVerVirtualBoundaries() == 0));
  bool iBestMvBitsOk = false;
  bool iMvBitsOk = false;
#endif

  if ( imv > 0 && imv < 3 )
  {
    return;
  }

  AMVPInfo* pcAMVPInfo = &amvpInfo;

  CHECK(pcAMVPInfo->mvCand[riMVPIdx] != rcMvPred, "Invalid MV prediction candidate");

  if (pcAMVPInfo->numCand < 2)
  {
    return;
  }

  m_pcRdCost->setCostScale ( 0    );

  int iBestMVPIdx = riMVPIdx;

  Mv pred = rcMvPred;
  pred.changeTransPrecInternal2Amvr(imv);
  m_pcRdCost->setPredictor( pred );
  Mv mv = cMv;
  mv.changeTransPrecInternal2Amvr(imv);
  int iOrgMvBits = m_pcRdCost->getBitsOfVectorWithPredictor(mv.getHor(), mv.getVer(), 0);
  iOrgMvBits += m_auiMVPIdxCost[riMVPIdx][AMVP_MAX_NUM_CANDS];
  int iBestMvBits = iOrgMvBits;
#if GDR_ENABLED
  if (isEncodeGdrClean)
  {
    iBestMvBitsOk = pcAMVPInfo->mvSolid[riMVPIdx];
  }
#endif

  for (int iMVPIdx = 0; iMVPIdx < pcAMVPInfo->numCand; iMVPIdx++)
  {
    if (iMVPIdx == riMVPIdx)
    {
      continue;
    }

    pred = pcAMVPInfo->mvCand[iMVPIdx];
    pred.changeTransPrecInternal2Amvr(imv);
    m_pcRdCost->setPredictor( pred );
    int iMvBits = m_pcRdCost->getBitsOfVectorWithPredictor(mv.getHor(), mv.getVer(), 0);
    iMvBits += m_auiMVPIdxCost[iMVPIdx][AMVP_MAX_NUM_CANDS];

#if GDR_ENABLED
    bool allOk = (iMvBits < iBestMvBits);
    if (isEncodeGdrClean)
    {
      iMvBitsOk = pcAMVPInfo->mvSolid[iMVPIdx];
      if (iMvBitsOk)
      {
        allOk = (iBestMvBitsOk) ? (iMvBits < iBestMvBits) : true;
      }
      else
      {
        allOk = false;
      }
    }
#endif

#if GDR_ENABLED
    if (allOk)
#else
    if (iMvBits < iBestMvBits)
#endif
    {
      iBestMvBits = iMvBits;
      iBestMVPIdx = iMVPIdx;
#if GDR_ENABLED
      if (isEncodeGdrClean)
      {
        iBestMvBitsOk = iMvBitsOk;
      }
#endif
    }
  }

  if (iBestMVPIdx != riMVPIdx)  //if changed
  {
    rcMvPred = pcAMVPInfo->mvCand[iBestMVPIdx];

    riMVPIdx = iBestMVPIdx;
    uint32_t uiOrgBits = ruiBits;
    ruiBits = uiOrgBits - iOrgMvBits + iBestMvBits;
    ruiCost = (ruiCost - m_pcRdCost->getCost( uiOrgBits ))  + m_pcRdCost->getCost( ruiBits );
  }
}

Distortion InterSearch::xGetTemplateCost(const PredictionUnit &pu, PelUnitBuf &origBuf, PelUnitBuf &predBuf, Mv cMvCand,
                                         int iMVPIdx, int iMVPNum, RefPicList eRefPicList, int refIdx)
{
  Distortion uiCost = std::numeric_limits<Distortion>::max();

  const Picture *picRef = pu.cu->slice->getRefPic(eRefPicList, refIdx);
  clipMv( cMvCand, pu.cu->lumaPos(), pu.cu->lumaSize(), *pu.cs->sps, *pu.cs->pps );

  // prediction pattern
  const bool bi = pu.cu->slice->testWeightPred() && pu.cu->slice->getSliceType()==P_SLICE;

  xPredInterBlk(COMPONENT_Y, pu, picRef, cMvCand, predBuf, bi, pu.cu->slice->clpRng(COMPONENT_Y), false, false);

  if ( bi )
  {
    xWeightedPredictionUni(pu, predBuf, eRefPicList, predBuf, refIdx, m_maxCompIDToPred);
  }

  // calc distortion

  uiCost = m_pcRdCost->getDistPart(origBuf.Y(), predBuf.Y(), pu.cs->sps->getBitDepth(CHANNEL_TYPE_LUMA), COMPONENT_Y, DF_SAD);
  uiCost += m_pcRdCost->getCost( m_auiMVPIdxCost[iMVPIdx][iMVPNum] );

  return uiCost;
}

#if GDR_ENABLED
Distortion InterSearch::xGetAffineTemplateCost(PredictionUnit &pu, PelUnitBuf &origBuf, PelUnitBuf &predBuf,
                                               Mv acMvCand[3], int iMVPIdx, int iMVPNum, RefPicList eRefPicList,
                                               int refIdx, bool &rbOk)
#else
Distortion InterSearch::xGetAffineTemplateCost(PredictionUnit &pu, PelUnitBuf &origBuf, PelUnitBuf &predBuf,
                                               Mv acMvCand[3], int iMVPIdx, int iMVPNum, RefPicList eRefPicList,
                                               int refIdx)
#endif
{
  Distortion uiCost = std::numeric_limits<Distortion>::max();

  const Picture *picRef = pu.cu->slice->getRefPic(eRefPicList, refIdx);

  // prediction pattern
  const bool bi = pu.cu->slice->testWeightPred() && pu.cu->slice->getSliceType()==P_SLICE;
  Mv mv[3];
  memcpy(mv, acMvCand, sizeof(mv));
  m_iRefListIdx = eRefPicList;

#if GDR_ENABLED
  rbOk = xPredAffineBlk(COMPONENT_Y, pu, picRef, mv, predBuf, bi, pu.cu->slice->clpRng(COMPONENT_Y));
#else
  xPredAffineBlk(COMPONENT_Y, pu, picRef, mv, predBuf, bi, pu.cu->slice->clpRng(COMPONENT_Y));
#endif
  if( bi )
  {
    xWeightedPredictionUni(pu, predBuf, eRefPicList, predBuf, refIdx, m_maxCompIDToPred);
  }

  // calc distortion
  enum DFunc distFunc = (pu.cs->slice->getDisableSATDForRD()) ? DF_SAD : DF_HAD;
  uiCost  = m_pcRdCost->getDistPart( origBuf.Y(), predBuf.Y(), pu.cs->sps->getBitDepth(CHANNEL_TYPE_LUMA), COMPONENT_Y
    , distFunc
  );
  uiCost += m_pcRdCost->getCost( m_auiMVPIdxCost[iMVPIdx][iMVPNum] );
  DTRACE( g_trace_ctx, D_COMMON, " (%d) affineTemplateCost=%d\n", DTRACE_GET_COUNTER(g_trace_ctx,D_COMMON), uiCost );
  return uiCost;
}

#if GDR_ENABLED
void InterSearch::xMotionEstimation(PredictionUnit &pu, PelUnitBuf &origBuf, RefPicList eRefPicList, Mv &rcMvPred,
                                    int refIdxPred, Mv &rcMv, bool &rcMvSolid, int &riMVPIdx, uint32_t &ruiBits,
                                    Distortion &ruiCost, const AMVPInfo &amvpInfo, bool &rbCleanCandExist, bool bBi)
#else
void InterSearch::xMotionEstimation(PredictionUnit &pu, PelUnitBuf &origBuf, RefPicList eRefPicList, Mv &rcMvPred,
                                    int refIdxPred, Mv &rcMv, int &riMVPIdx, uint32_t &ruiBits, Distortion &ruiCost,
                                    const AMVPInfo &amvpInfo, bool bBi)
#endif
{
#if GDR_ENABLED
  if (pu.cu->cs->sps->getUseBcw() && pu.cu->BcwIdx != BCW_DEFAULT && !bBi
      && xReadBufferedUniMv(pu, eRefPicList, refIdxPred, rcMvPred, rcMv, rcMvSolid, ruiBits, ruiCost))
#else
  if (pu.cu->cs->sps->getUseBcw() && pu.cu->BcwIdx != BCW_DEFAULT && !bBi
      && xReadBufferedUniMv(pu, eRefPicList, refIdxPred, rcMvPred, rcMv, ruiBits, ruiCost))
#endif
  {
    return;
  }

  Mv cMvHalf, cMvQter;

  CHECK(eRefPicList >= MAX_NUM_REF_LIST_ADAPT_SR || refIdxPred >= int(MAX_IDX_ADAPT_SR),
        "Invalid reference picture list");
  m_iSearchRange = m_aaiAdaptSR[eRefPicList][refIdxPred];

  int    iSrchRng   = (bBi ? m_bipredSearchRange : m_iSearchRange);
  double fWeight    = 1.0;

  PelUnitBuf  origBufTmp = m_tmpStorageLCU.getBuf( UnitAreaRelative(*pu.cu, pu) );
  PelUnitBuf* pBuf       = &origBuf;

  if(bBi) // Bi-predictive ME
  {
    // NOTE: Other buf contains predicted signal from another direction
    PelUnitBuf otherBuf = m_tmpPredStorage[1 - (int)eRefPicList].getBuf( UnitAreaRelative(*pu.cu, pu ));
    origBufTmp.copyFrom(origBuf);
    origBufTmp.removeHighFreq( otherBuf, m_pcEncCfg->getClipForBiPredMeEnabled(), pu.cu->slice->clpRngs()
                              ,getBcwWeight( pu.cu->BcwIdx, eRefPicList )
                              );
    pBuf = &origBufTmp;

    fWeight = xGetMEDistortionWeight( pu.cu->BcwIdx, eRefPicList );
  }
  m_cDistParam.isBiPred = bBi;

  //  Search key pattern initialization
  CPelBuf  tmpPattern   = pBuf->Y();
  CPelBuf* pcPatternKey = &tmpPattern;

  m_lumaClpRng = pu.cs->slice->clpRng( COMPONENT_Y );

  bool    wrap = pu.cu->slice->getRefPic(eRefPicList, refIdxPred)->isWrapAroundEnabled(pu.cs->pps);
  CPelBuf buf  = pu.cu->slice->getRefPic(eRefPicList, refIdxPred)->getRecoBuf(pu.blocks[COMPONENT_Y], wrap);

  IntTZSearchStruct cStruct;
  cStruct.pcPatternKey  = pcPatternKey;
  cStruct.iRefStride    = buf.stride;
  cStruct.piRefY        = buf.buf;
  cStruct.imvShift = pu.cu->imv == IMV_HPEL ? 1 : (pu.cu->imv << 1);
  cStruct.useAltHpelIf = pu.cu->imv == IMV_HPEL;
  cStruct.inCtuSearch = false;
  cStruct.zeroMV = false;
  {
    if (m_useCompositeRef && pu.cs->slice->getRefPic(eRefPicList, refIdxPred)->longTerm)
    {
      cStruct.inCtuSearch = true;
    }
  }

  auto blkCache = dynamic_cast<CacheBlkInfoCtrl*>( m_modeCtrl );

  bool bQTBTMV  = false;
  bool bQTBTMV2 = false;
  Mv cIntMv;
  if( !bBi )
  {
    bool bValid = blkCache && blkCache->getMv(pu, eRefPicList, refIdxPred, cIntMv);
    if( bValid )
    {
      bQTBTMV2 = true;
      cIntMv.changePrecision( MV_PRECISION_INT, MV_PRECISION_INTERNAL);
    }
  }

  Mv predQuarter = rcMvPred;
  predQuarter.changePrecision(MV_PRECISION_INTERNAL, MV_PRECISION_QUARTER);
  m_pcRdCost->setPredictor( predQuarter );

  m_pcRdCost->setCostScale(2);

  {
    setWpScalingDistParam(refIdxPred, eRefPicList, pu.cu->slice);
  }
  m_currRefPicList = eRefPicList;
  m_currRefPicIndex = refIdxPred;
  m_skipFracME = false;
  //  Do integer search
  if( ( m_motionEstimationSearchMethod == MESEARCH_FULL ) || bBi || bQTBTMV )
  {
    cStruct.subShiftMode = m_pcEncCfg->getFastInterSearchMode() == FASTINTERSEARCH_MODE1 || m_pcEncCfg->getFastInterSearchMode() == FASTINTERSEARCH_MODE3 ? 2 : 0;
    m_pcRdCost->setDistParam(m_cDistParam, *cStruct.pcPatternKey, cStruct.piRefY, cStruct.iRefStride, m_lumaClpRng.bd, COMPONENT_Y, cStruct.subShiftMode);

    Mv bestInitMv = (bBi ? rcMv : rcMvPred);
    Mv cTmpMv = bestInitMv;

    clipMv( cTmpMv, pu.cu->lumaPos(), pu.cu->lumaSize(), *pu.cs->sps, *pu.cs->pps );
    cTmpMv.changePrecision(MV_PRECISION_INTERNAL, MV_PRECISION_INT);
    m_cDistParam.cur.buf = cStruct.piRefY + (cTmpMv.ver * cStruct.iRefStride) + cTmpMv.hor;
    Distortion uiBestSad = m_cDistParam.distFunc(m_cDistParam);
    uiBestSad += m_pcRdCost->getCostOfVectorWithPredictor(cTmpMv.hor, cTmpMv.ver, cStruct.imvShift);

    for (int i = 0; i < m_uniMvListSize; i++)
    {
      BlkUniMvInfo* curMvInfo = m_uniMvList + ((m_uniMvListIdx - 1 - i + m_uniMvListMaxSize) % (m_uniMvListMaxSize));

      int j = 0;
      for (; j < i; j++)
      {
        BlkUniMvInfo *prevMvInfo = m_uniMvList + ((m_uniMvListIdx - 1 - j + m_uniMvListMaxSize) % (m_uniMvListMaxSize));
        if (curMvInfo->uniMvs[eRefPicList][refIdxPred] == prevMvInfo->uniMvs[eRefPicList][refIdxPred])
        {
          break;
        }
      }
      if (j < i)
      {
        continue;
      }

      cTmpMv = curMvInfo->uniMvs[eRefPicList][refIdxPred];
      clipMv( cTmpMv, pu.cu->lumaPos(), pu.cu->lumaSize(), *pu.cs->sps, *pu.cs->pps );
      cTmpMv.changePrecision(MV_PRECISION_INTERNAL, MV_PRECISION_INT);
      m_cDistParam.cur.buf = cStruct.piRefY + (cTmpMv.ver * cStruct.iRefStride) + cTmpMv.hor;

      Distortion uiSad = m_cDistParam.distFunc(m_cDistParam);
      uiSad += m_pcRdCost->getCostOfVectorWithPredictor(cTmpMv.hor, cTmpMv.ver, cStruct.imvShift);
      if (uiSad < uiBestSad)
      {
        uiBestSad = uiSad;
        bestInitMv                                 = curMvInfo->uniMvs[eRefPicList][refIdxPred];
        m_cDistParam.maximumDistortionForEarlyExit = uiSad;
      }
    }

    if( !bQTBTMV )
    {
#if GDR_ENABLED
      xSetSearchRange(pu, bestInitMv, iSrchRng, cStruct.searchRange, cStruct, eRefPicList, refIdxPred);
#else
      xSetSearchRange(pu, bestInitMv, iSrchRng, cStruct.searchRange, cStruct);
#endif
    }
    xPatternSearch( cStruct, rcMv, ruiCost);
  }
  else if( bQTBTMV2 )
  {
    rcMv = cIntMv;

    cStruct.subShiftMode = ( !m_pcEncCfg->getRestrictMESampling() && m_pcEncCfg->getMotionEstimationSearchMethod() == MESEARCH_SELECTIVE ) ? 1 :
                            ( m_pcEncCfg->getFastInterSearchMode() == FASTINTERSEARCH_MODE1 || m_pcEncCfg->getFastInterSearchMode() == FASTINTERSEARCH_MODE3 ) ? 2 : 0;
    //RAMIRO: chama o xTZSearch
    xTZSearch(CUW, CUH, pu, eRefPicList, refIdxPred, cStruct, rcMv, ruiCost, nullptr, false, true);
  }
  else
  {
    cStruct.subShiftMode = ( !m_pcEncCfg->getRestrictMESampling() && m_pcEncCfg->getMotionEstimationSearchMethod() == MESEARCH_SELECTIVE ) ? 1 :
                            ( m_pcEncCfg->getFastInterSearchMode() == FASTINTERSEARCH_MODE1 || m_pcEncCfg->getFastInterSearchMode() == FASTINTERSEARCH_MODE3 ) ? 2 : 0;
    rcMv = rcMvPred;
    const Mv *pIntegerMv2Nx2NPred = 0;
    xPatternSearchFast(pu, eRefPicList, refIdxPred, cStruct, rcMv, ruiCost, pIntegerMv2Nx2NPred);
    if( blkCache )
    {
      blkCache->setMv(pu.cs->area, eRefPicList, refIdxPred, rcMv);
    }
    else
    {
      m_integerMv2Nx2N[eRefPicList][refIdxPred] = rcMv;
    }
  }

  DTRACE( g_trace_ctx, D_ME, "%d %d %d :MECostFPel<L%d,%d>: %d,%d,%dx%d, %d", DTRACE_GET_COUNTER( g_trace_ctx, D_ME ), pu.cu->slice->getPOC(), 0, ( int ) eRefPicList, ( int ) bBi, pu.Y().x, pu.Y().y, pu.Y().width, pu.Y().height, ruiCost );
  // sub-pel refinement for sub-pel resolution
  if ( pu.cu->imv == 0 || pu.cu->imv == IMV_HPEL )
  {
    if( m_pcEncCfg->getMCTSEncConstraint() )
    {
      Area curTileAreaSubPelRestricted = pu.cs->picture->mctsInfo.getTileAreaSubPelRestricted( pu );
      // Area adjustment, because subpel refinement is going to (x-1;y-1) direction
      curTileAreaSubPelRestricted.x += 1;
      curTileAreaSubPelRestricted.y += 1;
      curTileAreaSubPelRestricted.width -= 1;
      curTileAreaSubPelRestricted.height -= 1;
      if( ! MCTSHelper::checkMvIsNotInRestrictedArea( pu, rcMv, curTileAreaSubPelRestricted, MV_PRECISION_INT ) )
      {
        MCTSHelper::clipMvToArea( rcMv, pu.Y(), curTileAreaSubPelRestricted, *pu.cs->sps, 0 );
      }
    }
#if GDR_ENABLED
    xPatternSearchFracDIF(pu, eRefPicList, refIdxPred, cStruct, rcMv, cMvHalf, cMvQter, ruiCost, rbCleanCandExist);
#else
    xPatternSearchFracDIF(pu, eRefPicList, refIdxPred, cStruct, rcMv, cMvHalf, cMvQter, ruiCost);
#endif
    m_pcRdCost->setCostScale( 0 );
    rcMv <<= 2;
    rcMv  += ( cMvHalf <<= 1 );
    rcMv  += cMvQter;
    uint32_t uiMvBits = m_pcRdCost->getBitsOfVectorWithPredictor( rcMv.getHor(), rcMv.getVer(), cStruct.imvShift );
    ruiBits += uiMvBits;
    ruiCost = ( Distortion ) ( floor( fWeight * ( ( double ) ruiCost - ( double ) m_pcRdCost->getCost( uiMvBits ) ) ) + ( double ) m_pcRdCost->getCost( ruiBits ) );
    rcMv.changePrecision(MV_PRECISION_QUARTER, MV_PRECISION_INTERNAL);
  }
  else // integer refinement for integer-pel and 4-pel resolution
  {
    rcMv.changePrecision(MV_PRECISION_INT, MV_PRECISION_INTERNAL);
#if GDR_ENABLED
    xPatternSearchIntRefine(pu, cStruct, rcMv, rcMvPred, riMVPIdx, ruiBits, ruiCost, amvpInfo, fWeight, eRefPicList,
                            refIdxPred, rbCleanCandExist);
#else
    xPatternSearchIntRefine( pu, cStruct, rcMv, rcMvPred, riMVPIdx, ruiBits, ruiCost, amvpInfo, fWeight);
#endif
  }
  DTRACE(g_trace_ctx, D_ME, "   MECost<L%d,%d>: %6d (%d)  MV:%d,%d\n", (int)eRefPicList, (int)bBi, ruiCost, ruiBits, rcMv.getHor() << 2, rcMv.getVer() << 2);
}

void InterSearch::xSetSearchRange(const PredictionUnit &pu, const Mv &cMvPred, const int iSrchRng, SearchRange &sr,
                                  IntTZSearchStruct &cStruct
#if GDR_ENABLED
                                  ,
                                  RefPicList eRefPicList, int refIdx
#endif
)
{
  const int iMvShift = MV_FRACTIONAL_BITS_INTERNAL;
  Mv cFPMvPred = cMvPred;
  clipMv( cFPMvPred, pu.cu->lumaPos(), pu.cu->lumaSize(), *pu.cs->sps, *pu.cs->pps );

  Mv mvTL(cFPMvPred.getHor() - (iSrchRng << iMvShift), cFPMvPred.getVer() - (iSrchRng << iMvShift));
  Mv mvBR(cFPMvPred.getHor() + (iSrchRng << iMvShift), cFPMvPred.getVer() + (iSrchRng << iMvShift));
#if GDR_ENABLED
  if (m_pcEncCfg->getGdrEnabled())
  {
    bool isRefGdrPicture = pu.cs->slice->getRefPic(eRefPicList, refIdx)->cs->picHeader->getInGdrInterval();
    if (isRefGdrPicture)
    {
      mvTL = { cFPMvPred.getHor(), cFPMvPred.getVer() };
      mvBR = { cFPMvPred.getHor(), cFPMvPred.getVer() };

      const int lumaPixelAway = 4;
      const int chromaPixelAway = 5;

      const Position LastPos = pu.Y().bottomRight();

      const int iMvShift = MV_FRACTIONAL_BITS_INTERNAL;
      const int iMvLumaFrac = (1 << iMvShift);
      const int iMvChromaFrac = (iMvLumaFrac << 1);
      const int iFracOne = (1 << iMvShift);

      const bool isIntLumaMv = (cFPMvPred.getHor() % iMvLumaFrac) == 0;
      const bool isIntChromaMv = (cFPMvPred.getHor() % iMvChromaFrac) == 0;

      const int scaled_endx = pu.cs->slice->getRefPic(eRefPicList, refIdx)->cs->picHeader->getVirtualBoundariesPosX(0)
                              << iMvShift;

      const Position OrigFracPos = Position(LastPos.x << iMvShift, LastPos.y << iMvShift);
      const int last_luma_pos = ((OrigFracPos.x / iMvLumaFrac)   * iMvLumaFrac) + cFPMvPred.getHor() + (isIntLumaMv ? 0 : (lumaPixelAway << iMvShift));
      const int last_chroma_pos = ((OrigFracPos.x / iMvChromaFrac) * iMvChromaFrac) + cFPMvPred.getHor() + (isIntChromaMv ? 0 : (chromaPixelAway << iMvShift));

      const int last_pel_pos = std::max(last_luma_pos, last_chroma_pos);

      const int distance = Clip3(-(iSrchRng << iMvShift), (iSrchRng << iMvShift), scaled_endx - (last_pel_pos + iFracOne));


      int srLeft = cFPMvPred.getHor() - (iSrchRng << iMvShift);
      int srRight = cFPMvPred.getHor() + distance;
      int srTop = cFPMvPred.getVer() - (iSrchRng << iMvShift);
      int srBottom = cFPMvPred.getVer() + (iSrchRng << iMvShift);

      mvTL = { srLeft, srTop };
      mvBR = { srRight, srBottom };
    }
  }
#endif

  if (m_pcEncCfg->getMCTSEncConstraint())
  {
    MCTSHelper::clipMvToArea( mvTL, pu.Y(), pu.cs->picture->mctsInfo.getTileArea(), *pu.cs->sps );
    MCTSHelper::clipMvToArea( mvBR, pu.Y(), pu.cs->picture->mctsInfo.getTileArea(), *pu.cs->sps );
  }
  else
  {
    xClipMv(mvTL, pu.cu->lumaPos(), pu.cu->lumaSize(), *pu.cs->sps, *pu.cs->pps);
    xClipMv(mvBR, pu.cu->lumaPos(), pu.cu->lumaSize(), *pu.cs->sps, *pu.cs->pps);
  }

  mvTL.divideByPowerOf2( iMvShift );
  mvBR.divideByPowerOf2( iMvShift );

  sr.left   = mvTL.hor;
  sr.top    = mvTL.ver;
  sr.right  = mvBR.hor;
  sr.bottom = mvBR.ver;

  if (m_useCompositeRef && cStruct.inCtuSearch)
  {
    Position posRB = pu.Y().bottomRight();
    Position posTL = pu.Y().topLeft();
    const PreCalcValues *pcv = pu.cs->pcv;
    Position posRBinCTU(posRB.x & pcv->maxCUWidthMask, posRB.y & pcv->maxCUHeightMask);
    Position posLTinCTU = Position(posTL.x & pcv->maxCUWidthMask, posTL.y & pcv->maxCUHeightMask).offset(-4, -4);
    if (sr.left < -posLTinCTU.x)
    {
      sr.left = -posLTinCTU.x;
    }
    if (sr.top < -posLTinCTU.y)
    {
      sr.top = -posLTinCTU.y;
    }
    if (sr.right >((int)pcv->maxCUWidth - 4 - posRBinCTU.x))
    {
      sr.right = (int)pcv->maxCUWidth - 4 - posRBinCTU.x;
    }
    if (sr.bottom >((int)pcv->maxCUHeight - 4 - posRBinCTU.y))
    {
      sr.bottom = (int)pcv->maxCUHeight - 4 - posRBinCTU.y;
    }
    if (posLTinCTU.x == -4 || posLTinCTU.y == -4)
    {
      sr.left = sr.right = sr.bottom = sr.top = 0;
      cStruct.zeroMV = 1;
    }
    if (posRBinCTU.x == pcv->maxCUWidthMask || posRBinCTU.y == pcv->maxCUHeightMask)
    {
      sr.left = sr.right = sr.bottom = sr.top = 0;
      cStruct.zeroMV = 1;
    }
  }
}


void InterSearch::xPatternSearch( IntTZSearchStruct&    cStruct,
                                  Mv&            rcMv,
                                  Distortion&    ruiSAD )
{
  Distortion  uiSad;
  Distortion  uiSadBest = std::numeric_limits<Distortion>::max();
  int         iBestX = 0;
  int         iBestY = 0;

  //-- jclee for using the SAD function pointer
  m_pcRdCost->setDistParam( m_cDistParam, *cStruct.pcPatternKey, cStruct.piRefY, cStruct.iRefStride, m_lumaClpRng.bd, COMPONENT_Y, cStruct.subShiftMode );

  const SearchRange& sr = cStruct.searchRange;

  const Pel* piRef = cStruct.piRefY + (sr.top * cStruct.iRefStride);
  for ( int y = sr.top; y <= sr.bottom; y++ )
  {
    for ( int x = sr.left; x <= sr.right; x++ )
    {
      //  find min. distortion position
      m_cDistParam.cur.buf = piRef + x;

      uiSad = m_cDistParam.distFunc( m_cDistParam );

      // motion cost
      uiSad += m_pcRdCost->getCostOfVectorWithPredictor( x, y, cStruct.imvShift );

      if ( uiSad < uiSadBest )
      {
        uiSadBest = uiSad;
        iBestX    = x;
        iBestY    = y;
        m_cDistParam.maximumDistortionForEarlyExit = uiSad;
      }
    }
    piRef += cStruct.iRefStride;
  }
  rcMv.set( iBestX, iBestY );

  cStruct.uiBestSad = uiSadBest; // th for testing
  ruiSAD = uiSadBest - m_pcRdCost->getCostOfVectorWithPredictor( iBestX, iBestY, cStruct.imvShift );
  return;
}

void InterSearch::xPatternSearchFast(const PredictionUnit &pu, RefPicList eRefPicList, int refIdxPred,
                                     IntTZSearchStruct &cStruct, Mv &rcMv, Distortion &ruiSAD,
                                     const Mv *const pIntegerMv2Nx2NPred)
{
  switch ( m_motionEstimationSearchMethod )
  {
  case MESEARCH_DIAMOND:
    xTZSearch(CUW, CUH, pu, eRefPicList, refIdxPred, cStruct, rcMv, ruiSAD, pIntegerMv2Nx2NPred, false);
    break;

  case MESEARCH_SELECTIVE:
    xTZSearchSelective(pu, eRefPicList, refIdxPred, cStruct, rcMv, ruiSAD, pIntegerMv2Nx2NPred);
    break;

  case MESEARCH_DIAMOND_ENHANCED:
    xTZSearch(CUW, CUH, pu, eRefPicList, refIdxPred, cStruct, rcMv, ruiSAD, pIntegerMv2Nx2NPred, true);
    break;

  case MESEARCH_FULL: // shouldn't get here.
  default:
    break;
  }
}

//RAMIRO: Função xTZSearch
void InterSearch::xTZSearch(long int CU_W, long int CU_H, const PredictionUnit &pu, RefPicList eRefPicList, int refIdxPred,
                            IntTZSearchStruct &cStruct, Mv &rcMv, Distortion &ruiSAD,
                            const Mv *const pIntegerMv2Nx2NPred, const bool bExtendedSettings, const bool bFastSettings)
{
  //RAMIRO: contar as repetições do TZSearch
  n_TZSearch++;

  //RAMIRO: testa se o TZS é executado
  //testaTZS = 1;

  //RAMIRO: inicio do timer do TZSearch
  clock_t iniClockTZSearch, fimClockTZSearch;
  iniClockTZSearch = clock();

  const bool bUseRasterInFastMode                    = true; //toggle this to further reduce runtime

  const bool bUseAdaptiveRaster                      = bExtendedSettings;
  const int  iRaster                                 = (bFastSettings && bUseRasterInFastMode) ? 8 : 5;
  const bool bTestZeroVector                         = true && !bFastSettings;
  const bool bTestZeroVectorStart                    = bExtendedSettings;
  const bool bTestZeroVectorStop                     = false;
  const bool bFirstSearchDiamond                     = true;  // 1 = xTZ8PointDiamondSearch   0 = xTZ8PointSquareSearch
  const bool bFirstCornersForDiamondDist1            = bExtendedSettings;
  const bool bFirstSearchStop                        = m_pcEncCfg->getFastMEAssumingSmootherMVEnabled();
  const uint32_t uiFirstSearchRounds                     = bFastSettings ? (bUseRasterInFastMode?3:2) : 3;     // first search stop X rounds after best match (must be >=1)
  const bool bEnableRasterSearch                     = bFastSettings ? bUseRasterInFastMode : true;
  const bool bAlwaysRasterSearch                     = bExtendedSettings;  // true: BETTER but factor 2 slower
  const bool bRasterRefinementEnable                 = false; // enable either raster refinement or star refinement
  const bool bRasterRefinementDiamond                = false; // 1 = xTZ8PointDiamondSearch   0 = xTZ8PointSquareSearch
  const bool bRasterRefinementCornersForDiamondDist1 = bExtendedSettings;
  const bool bStarRefinementEnable                   = true;  // enable either star refinement or raster refinement
  const bool bStarRefinementDiamond                  = true;  // 1 = xTZ8PointDiamondSearch   0 = xTZ8PointSquareSearch
  const bool bStarRefinementCornersForDiamondDist1   = bExtendedSettings;
  const bool bStarRefinementStop                     = false || bFastSettings;
  const uint32_t uiStarRefinementRounds                  = 2;  // star refinement stop X rounds after best match (must be >=1)
  const bool bNewZeroNeighbourhoodTest               = bExtendedSettings;

//RAMIRO: etapa MVPrediction

//RAMIRO: inicio do timer do MVPrediction
clock_t iniClockMVPrediction, fimClockMVPrediction;
iniClockMVPrediction = clock();

//RAMIRO: contar as repetições do MVPrediction
n_MVPrediction++;

#if GDR_ENABLED
  const CodingStructure &cs = *pu.cs;
  const bool isEncodeGdrClean = cs.sps->getGDREnabledFlag() && cs.pcv->isEncoder && ((cs.picHeader->getInGdrInterval() && cs.isClean(pu.Y().topRight(), CHANNEL_TYPE_LUMA)) || (cs.picHeader->getNumVerVirtualBoundaries() == 0));
#endif
  int iSearchRange = m_iSearchRange;
  if( m_pcEncCfg->getMCTSEncConstraint() )
  {
    MCTSHelper::clipMvToArea( rcMv, pu.Y(), pu.cs->picture->mctsInfo.getTileArea(), *pu.cs->sps );
  }
  else
  {
    clipMv( rcMv, pu.cu->lumaPos(), pu.cu->lumaSize(), *pu.cs->sps, *pu.cs->pps );
  }
  rcMv.changePrecision(MV_PRECISION_INTERNAL, MV_PRECISION_QUARTER);
  rcMv.divideByPowerOf2(2);

  // init TZSearchStruct
  cStruct.uiBestSad = std::numeric_limits<Distortion>::max();

  //
  m_cDistParam.maximumDistortionForEarlyExit = cStruct.uiBestSad;
  m_pcRdCost->setDistParam( m_cDistParam, *cStruct.pcPatternKey, cStruct.piRefY, cStruct.iRefStride, m_lumaClpRng.bd, COMPONENT_Y, cStruct.subShiftMode );

  // distortion


  // set rcMv (Median predictor) as start point and as best point
  xTZSearchHelp( cStruct, rcMv.getHor(), rcMv.getVer(), 0, 0 );

  // test whether zero Mv is better start point than Median predictor
  if ( bTestZeroVector )
  {
    if ((rcMv.getHor() != 0 || rcMv.getVer() != 0) &&
      (0 != cStruct.iBestX || 0 != cStruct.iBestY))
    {
      // only test 0-vector if not obviously previously tested.
      xTZSearchHelp( cStruct, 0, 0, 0, 0 );
    }
  }

  SearchRange& sr = cStruct.searchRange;

  if (pIntegerMv2Nx2NPred != 0)
  {
    Mv integerMv2Nx2NPred = *pIntegerMv2Nx2NPred;
    integerMv2Nx2NPred.changePrecision(MV_PRECISION_INT, MV_PRECISION_INTERNAL);
    if( m_pcEncCfg->getMCTSEncConstraint() )
    {
      MCTSHelper::clipMvToArea( integerMv2Nx2NPred, pu.Y(), pu.cs->picture->mctsInfo.getTileArea(), *pu.cs->sps );
    }
    else
    {
      clipMv( integerMv2Nx2NPred, pu.cu->lumaPos(), pu.cu->lumaSize(), *pu.cs->sps, *pu.cs->pps );
    }
    integerMv2Nx2NPred.changePrecision(MV_PRECISION_INTERNAL, MV_PRECISION_QUARTER);
    integerMv2Nx2NPred.divideByPowerOf2(2);

    if ((rcMv != integerMv2Nx2NPred) &&
      (integerMv2Nx2NPred.getHor() != cStruct.iBestX || integerMv2Nx2NPred.getVer() != cStruct.iBestY))
    {
      // only test integerMv2Nx2NPred if not obviously previously tested.
      xTZSearchHelp( cStruct, integerMv2Nx2NPred.getHor(), integerMv2Nx2NPred.getVer(), 0, 0);
    }
  }

  for (int i = 0; i < m_uniMvListSize; i++)
  {
    BlkUniMvInfo* curMvInfo = m_uniMvList + ((m_uniMvListIdx - 1 - i + m_uniMvListMaxSize) % (m_uniMvListMaxSize));

    int j = 0;
    for (; j < i; j++)
    {
      BlkUniMvInfo *prevMvInfo = m_uniMvList + ((m_uniMvListIdx - 1 - j + m_uniMvListMaxSize) % (m_uniMvListMaxSize));
      if (curMvInfo->uniMvs[eRefPicList][refIdxPred] == prevMvInfo->uniMvs[eRefPicList][refIdxPred])
      {
        break;
      }
    }
    if (j < i)
    {
      continue;
    }

    Mv cTmpMv = curMvInfo->uniMvs[eRefPicList][refIdxPred];
    clipMv( cTmpMv, pu.cu->lumaPos(), pu.cu->lumaSize(), *pu.cs->sps, *pu.cs->pps );
    cTmpMv.changePrecision(MV_PRECISION_INTERNAL, MV_PRECISION_INT);
    m_cDistParam.cur.buf = cStruct.piRefY + (cTmpMv.ver * cStruct.iRefStride) + cTmpMv.hor;

    Distortion uiSad = m_cDistParam.distFunc(m_cDistParam);
    uiSad += m_pcRdCost->getCostOfVectorWithPredictor(cTmpMv.hor, cTmpMv.ver, cStruct.imvShift);
#if GDR_ENABLED
    bool allOk = (uiSad < cStruct.uiBestSad);

    if (isEncodeGdrClean)
    {
      Mv motion = cTmpMv;
      motion.changePrecision(MV_PRECISION_INT, MV_PRECISION_INTERNAL);
      bool cTmpMvOk = cs.isClean(pu.Y().bottomRight(), motion, eRefPicList, refIdxPred);

      Mv bestMv = { cStruct.iBestX, cStruct.iBestY };
      bestMv.changePrecision(MV_PRECISION_INT, MV_PRECISION_INTERNAL);
      bool bestMvOk = cs.isClean(pu.Y().bottomRight(), bestMv, eRefPicList, refIdxPred);

      if (cTmpMvOk)
      {
        allOk = (bestMvOk) ? (uiSad < cStruct.uiBestSad) : true;
      }
      else
      {
        allOk = false;
      }
    }

    if (allOk)
    {
      cStruct.uiBestSad = uiSad;
      cStruct.iBestX = cTmpMv.hor;
      cStruct.iBestY = cTmpMv.ver;
      m_cDistParam.maximumDistortionForEarlyExit = uiSad;
    }
#else
    if (uiSad < cStruct.uiBestSad)
    {
      cStruct.uiBestSad = uiSad;
      cStruct.iBestX = cTmpMv.hor;
      cStruct.iBestY = cTmpMv.ver;
      m_cDistParam.maximumDistortionForEarlyExit = uiSad;
    }
#endif
  }

  {
    // set search range
    Mv currBestMv(cStruct.iBestX, cStruct.iBestY );
    currBestMv <<= MV_FRACTIONAL_BITS_INTERNAL;
#if GDR_ENABLED
    xSetSearchRange(pu, currBestMv, m_iSearchRange >> (bFastSettings ? 1 : 0), sr, cStruct, eRefPicList, refIdxPred);
#else
    xSetSearchRange(pu, currBestMv, m_iSearchRange >> (bFastSettings ? 1 : 0), sr, cStruct);
#endif
  }
  if (m_pcEncCfg->getUseHashME() && (m_currRefPicList == 0 || pu.cu->slice->getList1IdxToList0Idx(m_currRefPicIndex) < 0))
  {
    int minSize = min(pu.cu->lumaSize().width, pu.cu->lumaSize().height);
    if (minSize < 128 && minSize >= 4)
    {
      int numberOfOtherMvps = m_numHashMVStoreds[m_currRefPicList][m_currRefPicIndex];
      for (int i = 0; i < numberOfOtherMvps; i++)
      {
        xTZSearchHelp(cStruct, m_hashMVStoreds[m_currRefPicList][m_currRefPicIndex][i].getHor(), m_hashMVStoreds[m_currRefPicList][m_currRefPicIndex][i].getVer(), 0, 0);
      }
      if (numberOfOtherMvps > 0)
      {
        // write out best match
        rcMv.set(cStruct.iBestX, cStruct.iBestY);
        ruiSAD = cStruct.uiBestSad - m_pcRdCost->getCostOfVectorWithPredictor(cStruct.iBestX, cStruct.iBestY, cStruct.imvShift);
        m_skipFracME = true;
        return;
      }
    }
  }

  
  //RAMIRO: fim do timer do MVPrediction
  fimClockMVPrediction = clock();
  totalClockMVPrediction += (fimClockMVPrediction - iniClockMVPrediction);

  //}

  // start search
  int  iDist = 0;
  int  iStartX = cStruct.iBestX;
  int  iStartY = cStruct.iBestY;

  //RAMIRO: extrair ruiSAD, cStruct.iBestX, cStruct.iBestY e se o TZS é executado
  #if EXTRA_FEATURES
        rui_SAD = ruiSAD;
        cStruct_iBestX = cStruct.iBestX;
        cStruct_iBestY = cStruct.iBestY;
  #endif

  const bool bBestCandidateZero = (cStruct.iBestX == 0) && (cStruct.iBestY == 0);

  //RAMIRO: inicio do timer das DTS TZS
  clock_t iniClock_DTs_TZS, fimClock_DTs_TZS;
  iniClock_DTs_TZS = clock();

  //RAMIRO: verifica se faz o TZS
  int aplicaTZS = 1;
  aplicaTZS = extracaoDados::aplicaDT(CU_W, CU_H, features_TZS);
  //aplicaTZS = extracaoDados::aplicaDT(cu.Y().width, cu.Y().height, features_TZS);

  //RAMIRO: fim do timer das DTS TZS
  fimClock_DTs_TZS = clock();
  totalClock_DTs_TZS += (fimClock_DTs_TZS - iniClock_DTs_TZS);

  //RAMIRO: etapa de executar ou pular a TZS (1 executa e 0 pula)
  if(aplicaTZS){

  TZS1++;

  //RAMIRO: etapa FirstSearch
  
  //RAMIRO: inicio do timer do FirstSearch
  clock_t iniClockFirstSearch, fimClockFirstSearch;
  iniClockFirstSearch = clock();

  //RAMIRO: contar as repetições do FirstSearch
  n_FirstSearch++;

  // first search around best position up to now.
  // The following works as a "subsampled/log" window search around the best candidate
  for ( iDist = 1; iDist <= iSearchRange; iDist*=2 )
  {
    if ( bFirstSearchDiamond == 1 )
    {
      xTZ8PointDiamondSearch ( cStruct, iStartX, iStartY, iDist, bFirstCornersForDiamondDist1 );
    }
    else
    {
      xTZ8PointSquareSearch  ( cStruct, iStartX, iStartY, iDist );
    }

    if ( bFirstSearchStop && ( cStruct.uiBestRound >= uiFirstSearchRounds ) ) // stop criterion
    {
      break;
    }
  }

  if (!bNewZeroNeighbourhoodTest)
  {
    // test whether zero Mv is a better start point than Median predictor
    if ( bTestZeroVectorStart && ((cStruct.iBestX != 0) || (cStruct.iBestY != 0)) )
    {
      xTZSearchHelp( cStruct, 0, 0, 0, 0 );
      if ( (cStruct.iBestX == 0) && (cStruct.iBestY == 0) )
      {
        // test its neighborhood
        for ( iDist = 1; iDist <= iSearchRange; iDist*=2 )
        {
          xTZ8PointDiamondSearch( cStruct, 0, 0, iDist, false );
          if ( bTestZeroVectorStop && (cStruct.uiBestRound > 0) ) // stop criterion
          {
            break;
          }
        }
      }
    }
  }
  else
  {
    // Test also zero neighbourhood but with half the range
    // It was reported that the original (above) search scheme using bTestZeroVectorStart did not
    // make sense since one would have already checked the zero candidate earlier
    // and thus the conditions for that test would have not been satisfied
    if (bTestZeroVectorStart == true && bBestCandidateZero != true)
    {
      for ( iDist = 1; iDist <= (iSearchRange >> 1); iDist*=2 )
      {
        xTZ8PointDiamondSearch( cStruct, 0, 0, iDist, false );
        if ( bTestZeroVectorStop && (cStruct.uiBestRound > 2) ) // stop criterion
        {
          break;
        }
      }
    }
  }

  //RAMIRO: fim do timer do FirstSearch
  fimClockFirstSearch = clock();
  totalClockFirstSearch += (fimClockFirstSearch - iniClockFirstSearch);

  //RAMIRO: etapa RasterSearch
  
  // calculate only 2 missing points instead 8 points if cStruct.uiBestDistance == 1
  if ( cStruct.uiBestDistance == 1 )
  {
    cStruct.uiBestDistance = 0;
    xTZ2PointSearch( cStruct );
  }

  // raster search if distance is too big
  if (bUseAdaptiveRaster)
  {

    //RAMIRO: inicio do timer do RasterSearch1
    clock_t iniClockRasterSearch1, fimClockRasterSearch1;
    iniClockRasterSearch1 = clock();

    int iWindowSize     = iRaster;
    SearchRange localsr = sr;

    //RAMIRO: teste do VTM mesmo se o RasterSearch vai ser pulado ou não
    if (!(bEnableRasterSearch && ( ((int)(cStruct.uiBestDistance) >= iRaster))))
    {

    //RAMIRO: contar as repetições do RasterSearch1
    n_RasterSearch1++;

      iWindowSize ++;
      localsr.left   /= 2;
      localsr.right  /= 2;
      localsr.top    /= 2;
      localsr.bottom /= 2;
    }
    cStruct.uiBestDistance = iWindowSize;
    for ( iStartY = localsr.top; iStartY <= localsr.bottom; iStartY += iWindowSize )
    {
      for ( iStartX = localsr.left; iStartX <= localsr.right; iStartX += iWindowSize )
      {
        xTZSearchHelp( cStruct, iStartX, iStartY, 0, iWindowSize );
      }
    }

    //RAMIRO: fim do timer do RasterSearch1
    fimClockRasterSearch1 = clock();
    totalClockRasterSearch1 += (fimClockRasterSearch1 - iniClockRasterSearch1);

  }
  else
  {

    //RAMIRO: inicio do timer do RasterSearch2
    clock_t iniClockRasterSearch2, fimClockRasterSearch2;
    iniClockRasterSearch2 = clock();

    //RAMIRO: teste do VTM mesmo se o RasterSearch vai ser pulado ou não
    if ( bEnableRasterSearch && ( ((int)(cStruct.uiBestDistance) >= iRaster) || bAlwaysRasterSearch ) )
    {

    //RAMIRO: contar as repetições do RasterSearch2
    n_RasterSearch2++;

      cStruct.uiBestDistance = iRaster;
      for ( iStartY = sr.top; iStartY <= sr.bottom; iStartY += iRaster )
      {
        for ( iStartX = sr.left; iStartX <= sr.right; iStartX += iRaster )
        {
          xTZSearchHelp( cStruct, iStartX, iStartY, 0, iRaster );
        }
      }
    }

    //RAMIRO: fim do timer do RasterSearch2
    fimClockRasterSearch2 = clock();
    totalClockRasterSearch2 += (fimClockRasterSearch2 - iniClockRasterSearch2);

  }
    
  //RAMIRO: etapa Refinement

  // raster refinement
  if ( bRasterRefinementEnable && cStruct.uiBestDistance > 0 )
  {

    //RAMIRO: contar as repetições do RasterRefinement
    n_RasterRefinement++;

    //RAMIRO: inicio do timer do RasterRefinement
    clock_t iniClockRasterRefinement, fimClockRasterRefinement;
    iniClockRasterRefinement = clock();

    while ( cStruct.uiBestDistance > 0 )
    {
      iStartX = cStruct.iBestX;
      iStartY = cStruct.iBestY;
      if ( cStruct.uiBestDistance > 1 )
      {
        iDist = cStruct.uiBestDistance >>= 1;
        if ( bRasterRefinementDiamond == 1 )
        {
          xTZ8PointDiamondSearch ( cStruct, iStartX, iStartY, iDist, bRasterRefinementCornersForDiamondDist1 );
        }
        else
        {
          xTZ8PointSquareSearch  ( cStruct, iStartX, iStartY, iDist );
        }
      }

      // calculate only 2 missing points instead 8 points if cStruct.uiBestDistance == 1
      if ( cStruct.uiBestDistance == 1 )
      {
        cStruct.uiBestDistance = 0;
        if ( cStruct.ucPointNr != 0 )
        {
          xTZ2PointSearch( cStruct );
        }
      }
    }

    //RAMIRO: fim do timer do RasterRefinement
    fimClockRasterRefinement = clock();
    totalClockRasterRefinement += (fimClockRasterRefinement - iniClockRasterRefinement);

  }

  // star refinement
  if ( bStarRefinementEnable && cStruct.uiBestDistance > 0 )
  {

    //RAMIRO: contar as repetições do StarRefinement
    n_StarRefinement++;

    //RAMIRO: inicio do timer do StarRefinement
    clock_t iniClockStarRefinement, fimClockStarRefinement;
    iniClockStarRefinement = clock();

    while ( cStruct.uiBestDistance > 0 )
    {
      iStartX = cStruct.iBestX;
      iStartY = cStruct.iBestY;
      cStruct.uiBestDistance = 0;
      cStruct.ucPointNr = 0;
      for ( iDist = 1; iDist < iSearchRange + 1; iDist*=2 )
      {
        if ( bStarRefinementDiamond == 1 )
        {
          xTZ8PointDiamondSearch ( cStruct, iStartX, iStartY, iDist, bStarRefinementCornersForDiamondDist1 );
        }
        else
        {
          xTZ8PointSquareSearch  ( cStruct, iStartX, iStartY, iDist );
        }
        if ( bStarRefinementStop && (cStruct.uiBestRound >= uiStarRefinementRounds) ) // stop criterion
        {
          break;
        }
      }

      // calculate only 2 missing points instead 8 points if cStrukt.uiBestDistance == 1
      if ( cStruct.uiBestDistance == 1 )
      {
        cStruct.uiBestDistance = 0;
        if ( cStruct.ucPointNr != 0 )
        {
          xTZ2PointSearch( cStruct );
        }
      }
    }

    //RAMIRO: fim do timer do StarRefinement
    fimClockStarRefinement = clock();
    totalClockStarRefinement += (fimClockStarRefinement - iniClockStarRefinement);
  }

  }//RAMIRO: fim do aplicaTZS
  else
  {
      TZS0++;
  }

  // write out best match
  rcMv.set( cStruct.iBestX, cStruct.iBestY );
  ruiSAD = cStruct.uiBestSad - m_pcRdCost->getCostOfVectorWithPredictor( cStruct.iBestX, cStruct.iBestY, cStruct.imvShift );

 //RAMIRO: fim do timer do TZSearch
fimClockTZSearch = clock();
totalClockTZSearch += (fimClockTZSearch - iniClockTZSearch);
}

//RAMIRO: Função xTZSearchSelective
void InterSearch::xTZSearchSelective(const PredictionUnit &pu, RefPicList eRefPicList, int refIdxPred,
                                     IntTZSearchStruct &cStruct, Mv &rcMv, Distortion &ruiSAD,
                                     const Mv *const pIntegerMv2Nx2NPred)
{

  //RAMIRO: contar as repetições do TZSearchSelective
  n_TZSearchSelective++;

  //RAMIRO: inicio do timer do TZSearchSelective
  clock_t iniClockTZSearchSelective, fimClockTZSearchSelective;
  iniClockTZSearchSelective = clock();

  const bool bTestZeroVector          = true;
  const bool bEnableRasterSearch      = true;
  const bool bAlwaysRasterSearch      = false;  // 1: BETTER but factor 15x slower
  const bool bStarRefinementEnable    = true;   // enable either star refinement or raster refinement
  const bool bStarRefinementDiamond   = true;   // 1 = xTZ8PointDiamondSearch   0 = xTZ8PointSquareSearch
  const bool bStarRefinementStop      = false;
  const uint32_t uiStarRefinementRounds   = 2;  // star refinement stop X rounds after best match (must be >=1)
  const int  iSearchRange             = m_iSearchRange;
  const int  iSearchRangeInitial      = m_iSearchRange >> 2;
  const int  uiSearchStep             = 4;
  const int  iMVDistThresh            = 8;

  int   iStartX                 = 0;
  int   iStartY                 = 0;
  int   iDist                   = 0;

  clipMv( rcMv, pu.cu->lumaPos(), pu.cu->lumaSize(), *pu.cs->sps, *pu.cs->pps );
  rcMv.changePrecision(MV_PRECISION_INTERNAL, MV_PRECISION_QUARTER);
  rcMv.divideByPowerOf2(2);

  // init TZSearchStruct
  cStruct.uiBestSad = std::numeric_limits<Distortion>::max();
  cStruct.iBestX = 0;
  cStruct.iBestY = 0;

  m_cDistParam.maximumDistortionForEarlyExit = cStruct.uiBestSad;
  m_pcRdCost->setDistParam( m_cDistParam, *cStruct.pcPatternKey, cStruct.piRefY, cStruct.iRefStride, m_lumaClpRng.bd, COMPONENT_Y, cStruct.subShiftMode );


  // set rcMv (Median predictor) as start point and as best point
  xTZSearchHelp( cStruct, rcMv.getHor(), rcMv.getVer(), 0, 0 );

  // test whether zero Mv is better start point than Median predictor
  if ( bTestZeroVector )
  {
    xTZSearchHelp( cStruct, 0, 0, 0, 0 );
  }

  SearchRange& sr = cStruct.searchRange;

  if ( pIntegerMv2Nx2NPred != 0 )
  {
    Mv integerMv2Nx2NPred = *pIntegerMv2Nx2NPred;
    integerMv2Nx2NPred.changePrecision(MV_PRECISION_INT, MV_PRECISION_INTERNAL);
    clipMv( integerMv2Nx2NPred, pu.cu->lumaPos(), pu.cu->lumaSize(), *pu.cs->sps, *pu.cs->pps );
    integerMv2Nx2NPred.changePrecision(MV_PRECISION_INTERNAL, MV_PRECISION_QUARTER);
    integerMv2Nx2NPred.divideByPowerOf2(2);

    xTZSearchHelp( cStruct, integerMv2Nx2NPred.getHor(), integerMv2Nx2NPred.getVer(), 0, 0);
  }

  for (int i = 0; i < m_uniMvListSize; i++)
  {
    BlkUniMvInfo* curMvInfo = m_uniMvList + ((m_uniMvListIdx - 1 - i + m_uniMvListMaxSize) % (m_uniMvListMaxSize));

    int j = 0;
    for (; j < i; j++)
    {
      BlkUniMvInfo *prevMvInfo = m_uniMvList + ((m_uniMvListIdx - 1 - j + m_uniMvListMaxSize) % (m_uniMvListMaxSize));
      if (curMvInfo->uniMvs[eRefPicList][refIdxPred] == prevMvInfo->uniMvs[eRefPicList][refIdxPred])
      {
        break;
      }
    }
    if (j < i)
    {
      continue;
    }

    Mv cTmpMv = curMvInfo->uniMvs[eRefPicList][refIdxPred];
    clipMv( cTmpMv, pu.cu->lumaPos(), pu.cu->lumaSize(), *pu.cs->sps, *pu.cs->pps );
    cTmpMv.changePrecision(MV_PRECISION_INTERNAL, MV_PRECISION_INT);
    m_cDistParam.cur.buf = cStruct.piRefY + (cTmpMv.ver * cStruct.iRefStride) + cTmpMv.hor;

    Distortion uiSad = m_cDistParam.distFunc(m_cDistParam);
    uiSad += m_pcRdCost->getCostOfVectorWithPredictor(cTmpMv.hor, cTmpMv.ver, cStruct.imvShift);
    if (uiSad < cStruct.uiBestSad)
    {
      cStruct.uiBestSad = uiSad;
      cStruct.iBestX = cTmpMv.hor;
      cStruct.iBestY = cTmpMv.ver;
      m_cDistParam.maximumDistortionForEarlyExit = uiSad;
    }
  }

  {
    // set search range
    Mv currBestMv(cStruct.iBestX, cStruct.iBestY );
    currBestMv <<= 2;
#if GDR_ENABLED
    xSetSearchRange(pu, currBestMv, m_iSearchRange, sr, cStruct, eRefPicList, refIdxPred);
#else
    xSetSearchRange( pu, currBestMv, m_iSearchRange, sr, cStruct );
#endif
  }
  if (m_pcEncCfg->getUseHashME() && (m_currRefPicList == 0 || pu.cu->slice->getList1IdxToList0Idx(m_currRefPicIndex) < 0))
  {
    int minSize = min(pu.cu->lumaSize().width, pu.cu->lumaSize().height);
    if (minSize < 128 && minSize >= 4)
    {
      int numberOfOtherMvps = m_numHashMVStoreds[m_currRefPicList][m_currRefPicIndex];
      for (int i = 0; i < numberOfOtherMvps; i++)
      {
        xTZSearchHelp(cStruct, m_hashMVStoreds[m_currRefPicList][m_currRefPicIndex][i].getHor(), m_hashMVStoreds[m_currRefPicList][m_currRefPicIndex][i].getVer(), 0, 0);
      }
      if (numberOfOtherMvps > 0)
      {
        // write out best match
        rcMv.set(cStruct.iBestX, cStruct.iBestY);
        ruiSAD = cStruct.uiBestSad - m_pcRdCost->getCostOfVectorWithPredictor(cStruct.iBestX, cStruct.iBestY, cStruct.imvShift);
        m_skipFracME = true;
        return;
      }
    }
  }

  // Initial search

  int iBestX = cStruct.iBestX;
  int iBestY = cStruct.iBestY;
  int iFirstSrchRngHorLeft    = ((iBestX - iSearchRangeInitial) > sr.left)   ? (iBestX - iSearchRangeInitial) : sr.left;
  int iFirstSrchRngVerTop     = ((iBestY - iSearchRangeInitial) > sr.top)    ? (iBestY - iSearchRangeInitial) : sr.top;
  int iFirstSrchRngHorRight   = ((iBestX + iSearchRangeInitial) < sr.right)  ? (iBestX + iSearchRangeInitial) : sr.right;
  int iFirstSrchRngVerBottom  = ((iBestY + iSearchRangeInitial) < sr.bottom) ? (iBestY + iSearchRangeInitial) : sr.bottom;

  for ( iStartY = iFirstSrchRngVerTop; iStartY <= iFirstSrchRngVerBottom; iStartY += uiSearchStep )
  {
    for ( iStartX = iFirstSrchRngHorLeft; iStartX <= iFirstSrchRngHorRight; iStartX += uiSearchStep )
    {
      xTZSearchHelp( cStruct, iStartX, iStartY, 0, 0 );
      xTZ8PointDiamondSearch ( cStruct, iStartX, iStartY, 1, false );
      xTZ8PointDiamondSearch ( cStruct, iStartX, iStartY, 2, false );
    }
  }

  int iMaxMVDistToPred = (abs(cStruct.iBestX - iBestX) > iMVDistThresh || abs(cStruct.iBestY - iBestY) > iMVDistThresh);

  //full search with early exit if MV is distant from predictors
  if ( bEnableRasterSearch && (iMaxMVDistToPred || bAlwaysRasterSearch) )
  {
    for ( iStartY = sr.top; iStartY <= sr.bottom; iStartY += 1 )
    {
      for ( iStartX = sr.left; iStartX <= sr.right; iStartX += 1 )
      {
        xTZSearchHelp( cStruct, iStartX, iStartY, 0, 1 );
      }
    }
  }
  //Smaller MV, refine around predictor
  else if ( bStarRefinementEnable && cStruct.uiBestDistance > 0 )
  {
    // start refinement
    while ( cStruct.uiBestDistance > 0 )
    {
      iStartX = cStruct.iBestX;
      iStartY = cStruct.iBestY;
      cStruct.uiBestDistance = 0;
      cStruct.ucPointNr = 0;
      for ( iDist = 1; iDist < iSearchRange + 1; iDist*=2 )
      {
        if ( bStarRefinementDiamond == 1 )
        {
          xTZ8PointDiamondSearch ( cStruct, iStartX, iStartY, iDist, false );
        }
        else
        {
          xTZ8PointSquareSearch  ( cStruct, iStartX, iStartY, iDist );
        }
        if ( bStarRefinementStop && (cStruct.uiBestRound >= uiStarRefinementRounds) ) // stop criterion
        {
          break;
        }
      }

      // calculate only 2 missing points instead 8 points if cStrukt.uiBestDistance == 1
      if ( cStruct.uiBestDistance == 1 )
      {
        cStruct.uiBestDistance = 0;
        if ( cStruct.ucPointNr != 0 )
        {
          xTZ2PointSearch( cStruct );
        }
      }
    }
  }

  // write out best match
  rcMv.set( cStruct.iBestX, cStruct.iBestY );
  ruiSAD = cStruct.uiBestSad - m_pcRdCost->getCostOfVectorWithPredictor( cStruct.iBestX, cStruct.iBestY, cStruct.imvShift );

//RAMIRO: fim do timer do TZSearchSelective
fimClockTZSearchSelective = clock();
totalClockTZSearchSelective += (fimClockTZSearchSelective - iniClockTZSearchSelective);
}

#if GDR_ENABLED
void InterSearch::xPatternSearchIntRefine(PredictionUnit &pu, IntTZSearchStruct &cStruct, Mv &rcMv, Mv &rcMvPred,
                                          int &riMVPIdx, uint32_t &ruiBits, Distortion &ruiCost,
                                          const AMVPInfo &amvpInfo, double fWeight, RefPicList eRefPicList,
                                          int refIdxPred, bool &rbCleanCandExist)
#else
void InterSearch::xPatternSearchIntRefine(PredictionUnit& pu, IntTZSearchStruct&  cStruct, Mv& rcMv, Mv& rcMvPred, int& riMVPIdx, uint32_t& ruiBits, Distortion& ruiCost, const AMVPInfo& amvpInfo, double fWeight)
#endif
{
#if GDR_ENABLED
  const CodingStructure &cs = *pu.cs;
  const bool isEncodeGdrClean = cs.sps->getGDREnabledFlag() && cs.pcv->isEncoder && ((cs.picHeader->getInGdrInterval() && cs.isClean(pu.Y().topRight(), CHANNEL_TYPE_LUMA)) || (cs.picHeader->getNumVerVirtualBoundaries() == 0));
#endif

  CHECK( pu.cu->imv == 0 || pu.cu->imv == IMV_HPEL , "xPatternSearchIntRefine(): Sub-pel MV used.");
  CHECK( amvpInfo.mvCand[riMVPIdx] != rcMvPred, "xPatternSearchIntRefine(): MvPred issue.");

  const SPS &sps = *pu.cs->sps;
  m_pcRdCost->setDistParam(m_cDistParam, *cStruct.pcPatternKey, cStruct.piRefY, cStruct.iRefStride, m_lumaClpRng.bd, COMPONENT_Y, 0, 1, m_pcEncCfg->getUseHADME() && !pu.cs->slice->getDisableSATDForRD());

  // -> set MV scale for cost calculation to QPEL (0)
  m_pcRdCost->setCostScale ( 0 );

  Distortion  dist, uiSATD = 0;
  Distortion  uiBestDist  = std::numeric_limits<Distortion>::max();
  // subtract old MVP costs because costs for all newly tested MVPs are added in here
  ruiBits -= m_auiMVPIdxCost[riMVPIdx][AMVP_MAX_NUM_CANDS];

  Mv cBestMv = rcMv;
  Mv cBaseMvd[2];
  int iBestBits = 0;
  int iBestMVPIdx = riMVPIdx;
  Mv testPos[9] = { { 0, 0}, { -1, -1},{ -1, 0},{ -1, 1},{ 0, -1},{ 0, 1},{ 1, -1},{ 1, 0},{ 1, 1} };


  cBaseMvd[0] = (rcMv - amvpInfo.mvCand[0]);
  cBaseMvd[1] = (rcMv - amvpInfo.mvCand[1]);
  CHECK( (cBaseMvd[0].getHor() & 0x03) != 0 || (cBaseMvd[0].getVer() & 0x03) != 0 , "xPatternSearchIntRefine(): AMVP cand 0 Mvd issue.");
  CHECK( (cBaseMvd[1].getHor() & 0x03) != 0 || (cBaseMvd[1].getVer() & 0x03) != 0 , "xPatternSearchIntRefine(): AMVP cand 1 Mvd issue.");

  cBaseMvd[0].roundTransPrecInternal2Amvr(pu.cu->imv);
  cBaseMvd[1].roundTransPrecInternal2Amvr(pu.cu->imv);

  // test best integer position and all 8 neighboring positions
#if GDR_ENABLED
  bool allOk = true;
  bool diskOk       = false;
  bool uiBestDistOk = false;
#endif
  for (int pos = 0; pos < 9; pos ++)
  {
    Mv cTestMv[2];
    // test both AMVP candidates for each position
    for (int iMVPIdx = 0; iMVPIdx < amvpInfo.numCand; iMVPIdx++)
    {
      cTestMv[iMVPIdx] = testPos[pos];
      cTestMv[iMVPIdx].changeTransPrecAmvr2Internal(pu.cu->imv);
      cTestMv[iMVPIdx] += cBaseMvd[iMVPIdx];
      cTestMv[iMVPIdx] += amvpInfo.mvCand[iMVPIdx];

      // MCTS and IMV
      if( m_pcEncCfg->getMCTSEncConstraint() )
      {
        Mv cTestMVRestr = cTestMv[iMVPIdx];
        MCTSHelper::clipMvToArea( cTestMVRestr, pu.cu->Y(), pu.cs->picture->mctsInfo.getTileAreaIntPelRestricted( pu ), *pu.cs->sps );

        if( cTestMVRestr != cTestMv[iMVPIdx] )
        {
          // Skip this IMV pos, cause clipping affects IMV scaling
          continue;
        }
      }
      if ( iMVPIdx == 0 || cTestMv[0] != cTestMv[1])
      {
        Mv cTempMV = cTestMv[iMVPIdx];
        if( !m_pcEncCfg->getMCTSEncConstraint() )
        {
          clipMv( cTempMV, pu.cu->lumaPos(), pu.cu->lumaSize(), sps, *pu.cs->pps );
        }
        m_cDistParam.cur.buf = cStruct.piRefY  + cStruct.iRefStride * (cTempMV.getVer() >>  MV_FRACTIONAL_BITS_INTERNAL) + (cTempMV.getHor() >> MV_FRACTIONAL_BITS_INTERNAL);
        dist = uiSATD = (Distortion)(m_cDistParam.distFunc(m_cDistParam) * fWeight);
      }
      else
      {
        dist = uiSATD;
      }

      int iMvBits = m_auiMVPIdxCost[iMVPIdx][AMVP_MAX_NUM_CANDS];
      Mv pred = amvpInfo.mvCand[iMVPIdx];
      pred.changeTransPrecInternal2Amvr(pu.cu->imv);
      m_pcRdCost->setPredictor( pred );
      Mv mv = cTestMv[iMVPIdx];
      mv.changeTransPrecInternal2Amvr(pu.cu->imv);
      iMvBits += m_pcRdCost->getBitsOfVectorWithPredictor( mv.getHor(), mv.getVer(), 0 );
      dist += m_pcRdCost->getCost(iMvBits);

#if GDR_ENABLED
      allOk = (dist < uiBestDist);
      if (isEncodeGdrClean)
      {
        bool isSolid = amvpInfo.mvSolid[iMVPIdx];
        bool isValid = cs.isClean(pu.Y().bottomRight(), cTestMv[iMVPIdx], eRefPicList, refIdxPred);

        diskOk = isSolid && isValid;
        if (diskOk)
        {
          allOk = (uiBestDistOk) ? (dist < uiBestDist) : true;
        }
        else
        {
          allOk = false;
        }
      }
#endif

#if GDR_ENABLED
      if (allOk)
#else
      if (dist < uiBestDist)
#endif
      {
        uiBestDist  = dist;
        cBestMv = cTestMv[iMVPIdx];
        iBestMVPIdx = iMVPIdx;
        iBestBits = iMvBits;
#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          uiBestDistOk     = diskOk;
          rbCleanCandExist = true;
        }
#endif
      }
    }
  }
  if( uiBestDist == std::numeric_limits<Distortion>::max() )
  {
    ruiCost = std::numeric_limits<Distortion>::max();
    return;
  }

  rcMv = cBestMv;
  rcMvPred = amvpInfo.mvCand[iBestMVPIdx];
  riMVPIdx = iBestMVPIdx;
  m_pcRdCost->setPredictor( rcMvPred );

  ruiBits += iBestBits;
  // taken from JEM 5.0
  // verify since it makes no sence to subtract Lamda*(Rmvd+Rmvpidx) from D+Lamda(Rmvd)
  // this would take the rate for the MVP idx out of the cost calculation
  // however this rate is always 1 so impact is small
  ruiCost = uiBestDist - m_pcRdCost->getCost(iBestBits) + m_pcRdCost->getCost(ruiBits);
  // taken from JEM 5.0
  // verify since it makes no sense to add rate for MVDs twicce

  return;
}

void InterSearch::xPatternSearchFracDIF(const PredictionUnit &pu, RefPicList eRefPicList, int refIdx,
                                        IntTZSearchStruct &cStruct, const Mv &rcMvInt, Mv &rcMvHalf, Mv &rcMvQter,
                                        Distortion &ruiCost
#if GDR_ENABLED
                                        ,
                                        bool &rbCleanCandExist
#endif
)
{

  //  Reference pattern initialization (integer scale)
  int     offset = rcMvInt.getHor() + rcMvInt.getVer() * cStruct.iRefStride;
  CPelBuf cPatternRoi(cStruct.piRefY + offset, cStruct.iRefStride, *cStruct.pcPatternKey);
  if (m_skipFracME)
  {
    Mv baseRefMv(0, 0);
    rcMvHalf.setZero();
    m_pcRdCost->setCostScale(0);
    xExtDIFUpSamplingH(&cPatternRoi, cStruct.useAltHpelIf);
    rcMvQter = rcMvInt;   rcMvQter <<= 2;    // for mv-cost
#if GDR_ENABLED
    ruiCost = xPatternRefinement(pu, eRefPicList, refIdx, cStruct.pcPatternKey, baseRefMv, 1, rcMvQter,
                                 !pu.cs->slice->getDisableSATDForRD(), rbCleanCandExist);
#else
    ruiCost = xPatternRefinement(cStruct.pcPatternKey, baseRefMv, 1, rcMvQter, !pu.cs->slice->getDisableSATDForRD());
#endif
    return;
  }


  if (cStruct.imvShift > IMV_FPEL || (m_useCompositeRef && cStruct.zeroMV))
  {
    m_pcRdCost->setDistParam(m_cDistParam, *cStruct.pcPatternKey, cStruct.piRefY + offset, cStruct.iRefStride,
                             m_lumaClpRng.bd, COMPONENT_Y, 0, 1,
                             m_pcEncCfg->getUseHADME() && !pu.cs->slice->getDisableSATDForRD());
    ruiCost = m_cDistParam.distFunc( m_cDistParam );
    ruiCost += m_pcRdCost->getCostOfVectorWithPredictor( rcMvInt.getHor(), rcMvInt.getVer(), cStruct.imvShift );
    return;
  }

  //  Half-pel refinement
  m_pcRdCost->setCostScale(1);
  xExtDIFUpSamplingH(&cPatternRoi, cStruct.useAltHpelIf);

  rcMvHalf = rcMvInt;   rcMvHalf <<= 1;    // for mv-cost
  Mv baseRefMv(0, 0);
#if GDR_ENABLED
  ruiCost = xPatternRefinement(pu, eRefPicList, refIdx, cStruct.pcPatternKey, baseRefMv, 2, rcMvHalf,
                               (!pu.cs->slice->getDisableSATDForRD()), rbCleanCandExist);
#else
  ruiCost = xPatternRefinement(cStruct.pcPatternKey, baseRefMv, 2, rcMvHalf, (!pu.cs->slice->getDisableSATDForRD()));
#endif

  //  quarter-pel refinement
  if (cStruct.imvShift == IMV_OFF)
  {
    m_pcRdCost->setCostScale(0);
    xExtDIFUpSamplingQ(&cPatternRoi, rcMvHalf);
    baseRefMv = rcMvHalf;
    baseRefMv <<= 1;

    rcMvQter = rcMvInt;
    rcMvQter <<= 1;   // for mv-cost
    rcMvQter += rcMvHalf;
    rcMvQter <<= 1;
#if GDR_ENABLED
    ruiCost = xPatternRefinement(pu, eRefPicList, refIdx, cStruct.pcPatternKey, baseRefMv, 1, rcMvQter,
                                 (!pu.cs->slice->getDisableSATDForRD()), rbCleanCandExist);
#else
    ruiCost = xPatternRefinement(cStruct.pcPatternKey, baseRefMv, 1, rcMvQter, (!pu.cs->slice->getDisableSATDForRD()));
#endif
  }
}

Distortion InterSearch::xGetSymmetricCost( PredictionUnit& pu, PelUnitBuf& origBuf, RefPicList eCurRefPicList, const MvField& cCurMvField, MvField& cTarMvField, int bcwIdx )
{
  Distortion cost = std::numeric_limits<Distortion>::max();
  RefPicList eTarRefPicList = (RefPicList)(1 - (int)eCurRefPicList);

  // get prediction of eCurRefPicList
  PelUnitBuf predBufA = m_tmpPredStorage[eCurRefPicList].getBuf( UnitAreaRelative( *pu.cu, pu ) );
  const Picture* picRefA = pu.cu->slice->getRefPic( eCurRefPicList, cCurMvField.refIdx );
  Mv mvA = cCurMvField.mv;
  clipMv( mvA, pu.cu->lumaPos(), pu.cu->lumaSize(), *pu.cs->sps, *pu.cs->pps );
  if ( (mvA.hor & 15) == 0 && (mvA.ver & 15) == 0 )
  {
    Position offset = pu.blocks[COMPONENT_Y].pos().offset( mvA.getHor() >> 4, mvA.getVer() >> 4 );
    CPelBuf pelBufA = picRefA->getRecoBuf( CompArea( COMPONENT_Y, pu.chromaFormat, offset, pu.blocks[COMPONENT_Y].size() ), false );
    predBufA.bufs[0].buf = const_cast<Pel *>(pelBufA.buf);
    predBufA.bufs[0].stride = pelBufA.stride;
    predBufA.bufs[0].width = pelBufA.width;
    predBufA.bufs[0].height = pelBufA.height;
  }
  else
  {
    xPredInterBlk( COMPONENT_Y, pu, picRefA, mvA, predBufA, false, pu.cu->slice->clpRng( COMPONENT_Y ), false, false );
  }

  // get prediction of eTarRefPicList
  PelUnitBuf predBufB = m_tmpPredStorage[eTarRefPicList].getBuf( UnitAreaRelative( *pu.cu, pu ) );
  const Picture* picRefB = pu.cu->slice->getRefPic( eTarRefPicList, cTarMvField.refIdx );
  Mv mvB = cTarMvField.mv;
  clipMv( mvB, pu.cu->lumaPos(), pu.cu->lumaSize(), *pu.cs->sps, *pu.cs->pps );
  if ( (mvB.hor & 15) == 0 && (mvB.ver & 15) == 0 )
  {
    Position offset = pu.blocks[COMPONENT_Y].pos().offset( mvB.getHor() >> 4, mvB.getVer() >> 4 );
    CPelBuf pelBufB = picRefB->getRecoBuf( CompArea( COMPONENT_Y, pu.chromaFormat, offset, pu.blocks[COMPONENT_Y].size() ), false );
    predBufB.bufs[0].buf = const_cast<Pel *>(pelBufB.buf);
    predBufB.bufs[0].stride = pelBufB.stride;
  }
  else
  {
    xPredInterBlk( COMPONENT_Y, pu, picRefB, mvB, predBufB, false, pu.cu->slice->clpRng( COMPONENT_Y ), false, false );
  }

  PelUnitBuf bufTmp = m_tmpStorageLCU.getBuf( UnitAreaRelative( *pu.cu, pu ) );
  bufTmp.copyFrom( origBuf );
  bufTmp.removeHighFreq( predBufA, m_pcEncCfg->getClipForBiPredMeEnabled(), pu.cu->slice->clpRngs(), getBcwWeight( pu.cu->BcwIdx, eTarRefPicList ) );
  double fWeight = xGetMEDistortionWeight( pu.cu->BcwIdx, eTarRefPicList );

  // calc distortion
  DFunc distFunc = (!pu.cu->slice->getDisableSATDForRD()) ? DF_HAD : DF_SAD;
  cost = (Distortion)floor( fWeight * (double)m_pcRdCost->getDistPart( bufTmp.Y(), predBufB.Y(), pu.cs->sps->getBitDepth( CHANNEL_TYPE_LUMA ), COMPONENT_Y, distFunc ) );
  return(cost);
}

#if GDR_ENABLED
Distortion InterSearch::xSymmeticRefineMvSearch( PredictionUnit &pu, PelUnitBuf& origBuf, Mv& rcMvCurPred, Mv& rcMvTarPred
  , RefPicList eRefPicList, MvField& rCurMvField, MvField& rTarMvField, Distortion uiMinCost, int SearchPattern, int nSearchStepShift, uint32_t uiMaxSearchRounds, int bcwIdx, bool& rOk)
#else
Distortion InterSearch::xSymmeticRefineMvSearch( PredictionUnit &pu, PelUnitBuf& origBuf, Mv& rcMvCurPred, Mv& rcMvTarPred
  , RefPicList eRefPicList, MvField& rCurMvField, MvField& rTarMvField, Distortion uiMinCost, int SearchPattern, int nSearchStepShift, uint32_t uiMaxSearchRounds, int bcwIdx )
#endif
{
#if GDR_ENABLED
  const CodingStructure &cs = *pu.cs;
  const bool isEncodeGdrClean = cs.sps->getGDREnabledFlag() && cs.pcv->isEncoder && ((cs.picHeader->getInGdrInterval() && cs.isClean(pu.Y().topRight(), CHANNEL_TYPE_LUMA)) || (cs.picHeader->getNumVerVirtualBoundaries() == 0));
  bool uiCostOk;
  bool uiMinCostOk = rOk;
#endif
  const Mv mvSearchOffsetCross[4] = { Mv( 0 , 1 ) , Mv( 1 , 0 ) , Mv( 0 , -1 ) , Mv( -1 ,  0 ) };
  const Mv mvSearchOffsetSquare[8] = { Mv( -1 , 1 ) , Mv( 0 , 1 ) , Mv( 1 ,  1 ) , Mv( 1 ,  0 ) , Mv( 1 , -1 ) , Mv( 0 , -1 ) , Mv( -1 , -1 ) , Mv( -1 , 0 ) };
  const Mv mvSearchOffsetDiamond[8] = { Mv( 0 , 2 ) , Mv( 1 , 1 ) , Mv( 2 ,  0 ) , Mv( 1 , -1 ) , Mv( 0 , -2 ) , Mv( -1 , -1 ) , Mv( -2 ,  0 ) , Mv( -1 , 1 ) };
  const Mv mvSearchOffsetHexagon[6] = { Mv( 2 , 0 ) , Mv( 1 , 2 ) , Mv( -1 ,  2 ) , Mv( -2 ,  0 ) , Mv( -1 , -2 ) , Mv( 1 , -2 ) };

  int nDirectStart = 0, nDirectEnd = 0, nDirectRounding = 0, nDirectMask = 0;
  const Mv * pSearchOffset;
  if ( SearchPattern == 0 )
  {
    nDirectEnd = 3;
    nDirectRounding = 4;
    nDirectMask = 0x03;
    pSearchOffset = mvSearchOffsetCross;
  }
  else if ( SearchPattern == 1 )
  {
    nDirectEnd = 7;
    nDirectRounding = 8;
    nDirectMask = 0x07;
    pSearchOffset = mvSearchOffsetSquare;
  }
  else if ( SearchPattern == 2 )
  {
    nDirectEnd = 7;
    nDirectRounding = 8;
    nDirectMask = 0x07;
    pSearchOffset = mvSearchOffsetDiamond;
  }
  else if ( SearchPattern == 3 )
  {
    nDirectEnd = 5;
    pSearchOffset = mvSearchOffsetHexagon;
  }
  else
  {
    THROW( "Invalid search pattern" );
  }

  int nBestDirect;
  for ( uint32_t uiRound = 0; uiRound < uiMaxSearchRounds; uiRound++ )
  {
    nBestDirect = -1;
    MvField mvCurCenter = rCurMvField;
    for ( int nIdx = nDirectStart; nIdx <= nDirectEnd; nIdx++ )
    {
      int nDirect;
      if ( SearchPattern == 3 )
      {
        nDirect = nIdx < 0 ? nIdx + 6 : nIdx >= 6 ? nIdx - 6 : nIdx;
      }
      else
      {
        nDirect = (nIdx + nDirectRounding) & nDirectMask;
      }

      Mv mvOffset = pSearchOffset[nDirect];
      mvOffset <<= nSearchStepShift;
      MvField mvCand = mvCurCenter, mvPair;
      mvCand.mv += mvOffset;

      if( m_pcEncCfg->getMCTSEncConstraint() )
      {
        if( !( MCTSHelper::checkMvForMCTSConstraint( pu, mvCand.mv ) ) )
          continue; // Skip this this pos
      }
      // get MVD cost
      Mv pred = rcMvCurPred;
      pred.changeTransPrecInternal2Amvr(pu.cu->imv);
      m_pcRdCost->setPredictor( pred );
      m_pcRdCost->setCostScale( 0 );
      Mv mv = mvCand.mv;
      mv.changeTransPrecInternal2Amvr(pu.cu->imv);
      uint32_t uiMvBits = m_pcRdCost->getBitsOfVectorWithPredictor( mv.getHor(), mv.getVer(), 0 );
      Distortion uiCost = m_pcRdCost->getCost( uiMvBits );

#if GDR_ENABLED
      uiCostOk = cs.isClean(pu.Y().bottomRight(), mvCand.mv, eRefPicList, mvCand.refIdx);
#endif

      // get MVD pair and set target MV
      mvPair.refIdx = rTarMvField.refIdx;
      mvPair.mv.set( rcMvTarPred.hor - (mvCand.mv.hor - rcMvCurPred.hor), rcMvTarPred.ver - (mvCand.mv.ver - rcMvCurPred.ver) );
      if( m_pcEncCfg->getMCTSEncConstraint() )
      {
        if( !( MCTSHelper::checkMvForMCTSConstraint( pu, mvPair.mv ) ) )
          continue; // Skip this this pos
      }
      uiCost += xGetSymmetricCost( pu, origBuf, eRefPicList, mvCand, mvPair, bcwIdx );

#if GDR_ENABLED
      bool allOk = (uiCost < uiMinCost);
      if (isEncodeGdrClean)
      {
        bool curValid = cs.isClean(pu.Y().bottomRight(), mvCand.mv, (RefPicList)(eRefPicList), mvCand.refIdx);
        bool tarValid = cs.isClean(pu.Y().bottomRight(), mvPair.mv, (RefPicList)(1 - eRefPicList), mvPair.refIdx);
        allOk = curValid && tarValid;
      }
#endif

#if GDR_ENABLED
      if (allOk)
#else
      if ( uiCost < uiMinCost )
#endif
      {
        uiMinCost = uiCost;
        rCurMvField = mvCand;
        rTarMvField = mvPair;
        nBestDirect = nDirect;
#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          uiMinCostOk = uiCostOk;
        }
#endif
      }
    }

    if ( nBestDirect == -1 )
    {
      break;
    }
    int nStep = 1;
    if ( SearchPattern == 1 || SearchPattern == 2 )
    {
      nStep = 2 - (nBestDirect & 0x01);
    }
    nDirectStart = nBestDirect - nStep;
    nDirectEnd = nBestDirect + nStep;
  }

#if GDR_ENABLED
  rOk = uiMinCostOk;
#endif

  return(uiMinCost);
}


#if GDR_ENABLED
bool InterSearch::xSymmetricMotionEstimation(PredictionUnit& pu, PelUnitBuf& origBuf, Mv& rcMvCurPred, Mv& rcMvTarPred, RefPicList eRefPicList, MvField& rCurMvField, MvField& rTarMvField, Distortion& ruiCost, int bcwIdx, bool& ruiCostOk)
#else
void InterSearch::xSymmetricMotionEstimation( PredictionUnit& pu, PelUnitBuf& origBuf, Mv& rcMvCurPred, Mv& rcMvTarPred, RefPicList eRefPicList, MvField& rCurMvField, MvField& rTarMvField, Distortion& ruiCost, int bcwIdx )
#endif
{
  // Refine Search
  int nSearchStepShift = MV_FRACTIONAL_BITS_DIFF;
  int nDiamondRound = 8;
  int nCrossRound = 1;

  nSearchStepShift += pu.cu->imv == IMV_HPEL ? 1 : (pu.cu->imv << 1);
  nDiamondRound >>= pu.cu->imv;

#if GDR_ENABLED
  ruiCost = xSymmeticRefineMvSearch(pu, origBuf, rcMvCurPred, rcMvTarPred, eRefPicList, rCurMvField, rTarMvField, ruiCost, 2, nSearchStepShift, nDiamondRound, bcwIdx, ruiCostOk);
  ruiCost = xSymmeticRefineMvSearch(pu, origBuf, rcMvCurPred, rcMvTarPred, eRefPicList, rCurMvField, rTarMvField, ruiCost, 0, nSearchStepShift, nCrossRound, bcwIdx, ruiCostOk);
#else
  ruiCost = xSymmeticRefineMvSearch( pu, origBuf, rcMvCurPred, rcMvTarPred, eRefPicList, rCurMvField, rTarMvField, ruiCost, 2, nSearchStepShift, nDiamondRound, bcwIdx );
  ruiCost = xSymmeticRefineMvSearch( pu, origBuf, rcMvCurPred, rcMvTarPred, eRefPicList, rCurMvField, rTarMvField, ruiCost, 0, nSearchStepShift, nCrossRound, bcwIdx );
#endif

#if GDR_ENABLED
  return ruiCostOk;
#endif
}

void InterSearch::xPredAffineInterSearch( PredictionUnit&       pu,
                                          PelUnitBuf&           origBuf,
                                          int                   puIdx,
                                          uint32_t&                 lastMode,
                                          Distortion&           affineCost,
                                          Mv                    hevcMv[2][33]
#if GDR_ENABLED
                                        , bool                  hevcMvSolid[2][33]
#endif
                                        , Mv                    mvAffine4Para[2][33][3]
#if GDR_ENABLED
                                        , bool                  mvAffine4ParaSolid[2][33][3]
#endif
                                        , int                   refIdx4Para[2]
                                        , uint8_t               bcwIdx
                                        , bool                  enforceBcwPred
                                        , uint32_t              bcwIdxBits
                                         )
{
  const Slice &slice = *pu.cu->slice;

  affineCost = std::numeric_limits<Distortion>::max();

  Mv        cMvZero;
  Mv        aacMv[2][3];
  Mv        cMvBi[2][3];
  Mv        cMvTemp[2][33][3];

  int       iNumPredDir = slice.isInterP() ? 1 : 2;

  int mvNum = 2;
  mvNum = pu.cu->affineType ? 3 : 2;

  // Mvp
  Mv        cMvPred[2][33][3];
  Mv        cMvPredBi[2][33][3];
  int       aaiMvpIdxBi[2][33];
  int       aaiMvpIdx[2][33];
  int       aaiMvpNum[2][33];

#if GDR_ENABLED
  bool      aacMvSolid[2][3];
  bool      aacMvValid[2][3];

  bool      cMvTempSolid[2][33][3];
  bool      cMvTempValid[2][33][3];

  bool      cMvBiSolid[2][3];
  bool      cMvBiValid[2][3];

  bool      cMvPredSolid[2][33][3];
  bool      cMvPredBiSolid[2][33][3];

  bool      mvValidList1Solid[3];
  bool      mvValidList1Valid[3];

  bool      mvHevcSolid[3];

  const CodingStructure &cs = *pu.cs;
  const bool isEncodeGdrClean = cs.sps->getGDREnabledFlag() && cs.pcv->isEncoder && ((cs.picHeader->getInGdrInterval() && cs.isClean(pu.Y().topRight(), CHANNEL_TYPE_LUMA)) || (cs.picHeader->getNumVerVirtualBoundaries() == 0));
#endif
  AffineAMVPInfo aacAffineAMVPInfo[2][33];
  AffineAMVPInfo affiAMVPInfoTemp[2];

  int refIdx[2] = { 0, 0 };   // If un-initialized, may cause SEGV in bi-directional prediction iterative stage.
  int           iRefIdxBi[2];

  uint32_t mbBits[3] = { 1, 1, 0 };

  int           iRefStart, iRefEnd;

  int           bestBiPRefIdxL1 = 0;
  int           bestBiPMvpL1 = 0;
  Distortion biPDistTemp = std::numeric_limits<Distortion>::max();

#if GDR_ENABLED
  bool init_value = true;

  bool          allOk = true;
  bool          biPDistTempOk = init_value;
  bool          bestBiPDistOk = init_value;


  // if (isEncodeGdrClean)
  {
    iRefIdxBi[0] = -1;
    iRefIdxBi[1] = -1;
    memset(mvHevcSolid, init_value, sizeof(mvHevcSolid));

    // note : will have Solid problem if initialize to true
    memset(aacMvSolid, false, sizeof(aacMvSolid));
    memset(aacMvValid, false, sizeof(aacMvValid));

    memset(cMvBiSolid, init_value, sizeof(cMvBiSolid));
    memset(cMvBiValid, init_value, sizeof(cMvBiValid));

    memset(cMvTempSolid, init_value, sizeof(cMvTempSolid));
    memset(cMvTempValid, init_value, sizeof(cMvTempValid));

    memset(mvValidList1Solid, init_value, sizeof(mvValidList1Solid));
    memset(mvValidList1Valid, init_value, sizeof(mvValidList1Valid));

    // AffineAMVPInfo aacAffineAMVPInfo[2][33];
    ::memset(aacAffineAMVPInfo, 0, sizeof(aacAffineAMVPInfo));
    ::memset(affiAMVPInfoTemp, 0, sizeof(affiAMVPInfoTemp));

    for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 33; j++)
      {
        for (int k = 0; k < AMVP_MAX_NUM_CANDS_MEM; k++)
        {
          aacAffineAMVPInfo[i][j].mvSolidLT[k] = init_value;
          aacAffineAMVPInfo[i][j].mvSolidRT[k] = init_value;
          aacAffineAMVPInfo[i][j].mvSolidLB[k] = init_value;
        }
      }

      for (int k = 0; k < AMVP_MAX_NUM_CANDS_MEM; k++)
      {
        affiAMVPInfoTemp[i].mvSolidLT[k] = init_value;
        affiAMVPInfoTemp[i].mvSolidRT[k] = init_value;
        affiAMVPInfoTemp[i].mvSolidLB[k] = init_value;
      }
    }
  }

  bool bAnyClean = false;
#endif

  Distortion    uiCost[2] = { std::numeric_limits<Distortion>::max(), std::numeric_limits<Distortion>::max() };
  Distortion    uiCostBi  = std::numeric_limits<Distortion>::max();
  Distortion    costTemp;
#if GDR_ENABLED
  bool uiCostOk[2] = { init_value, init_value };
  bool uiCostBiOk = init_value;
  bool uiCostTempOk = init_value;
#endif

  uint32_t          bits[3] = { 0 };
  uint32_t          bitsTemp;
  Distortion    bestBiPDist = std::numeric_limits<Distortion>::max();

  Distortion    uiCostTempL0[MAX_NUM_REF];
  for (int iNumRef=0; iNumRef < MAX_NUM_REF; iNumRef++)
  {
    uiCostTempL0[iNumRef] = std::numeric_limits<Distortion>::max();
  }

#if GDR_ENABLED
  bool uiCostTempL0Ok[MAX_NUM_REF];
  for (int iNumRef = 0; iNumRef < MAX_NUM_REF; iNumRef++)
  {
    uiCostTempL0Ok[iNumRef] = true;
  }
#endif

  uint32_t uiBitsTempL0[MAX_NUM_REF];

  Mv            mvValidList1[4];
  int           refIdxValidList1 = 0;
  uint32_t          bitsValidList1 = MAX_UINT;
  Distortion costValidList1 = std::numeric_limits<Distortion>::max();
#if GDR_ENABLED
  bool costValidList1Ok = true;
#endif
  Mv            mvHevc[3];
  const bool affineAmvrEnabled = pu.cu->slice->getSPS()->getAffineAmvrEnabledFlag();
  int tryBipred = 0;
  WPScalingParam *wp0;
  WPScalingParam *wp1;
  xGetBlkBits(slice.isInterP(), puIdx, lastMode, mbBits);

  pu.cu->affine = true;
  pu.mergeFlag = false;
  pu.regularMergeFlag = false;
  if( bcwIdx != BCW_DEFAULT )
  {
    pu.cu->BcwIdx = bcwIdx;
  }

  //RAMIRO: Predicao Unidirecional
  // Uni-directional prediction
  for (int refList = 0; refList < iNumPredDir; refList++)
  {
    RefPicList eRefPicList = (refList ? REF_PIC_LIST_1 : REF_PIC_LIST_0);
    pu.interDir            = (refList ? 2 : 1);
    for (int refIdxTemp = 0; refIdxTemp < slice.getNumRefIdx(eRefPicList); refIdxTemp++)
    {
      // Get RefIdx bits
      bitsTemp = mbBits[refList];
      if ( slice.getNumRefIdx(eRefPicList) > 1 )
      {
        bitsTemp += refIdxTemp + 1;
        if (refIdxTemp == slice.getNumRefIdx(eRefPicList) - 1)
        {
          bitsTemp--;
        }
      }

      //RAMIRO: chama a funcao EstimateAffineAMVP
      // Do Affine AMVP
      xEstimateAffineAMVP(pu, affiAMVPInfoTemp[eRefPicList], origBuf, eRefPicList, refIdxTemp,
                          cMvPred[refList][refIdxTemp], &biPDistTemp);
      if ( affineAmvrEnabled )
      {
        biPDistTemp +=
          m_pcRdCost->getCost(xCalcAffineMVBits(pu, cMvPred[refList][refIdxTemp], cMvPred[refList][refIdxTemp]));
      }
      aaiMvpIdx[refList][refIdxTemp] = pu.mvpIdx[eRefPicList];
      aaiMvpNum[refList][refIdxTemp] = pu.mvpNum[eRefPicList];

#if GDR_ENABLED
      if (isEncodeGdrClean)
      {
        int mvpIdx                           = aaiMvpIdx[refList][refIdxTemp];
        cMvPredSolid[refList][refIdxTemp][0] = affiAMVPInfoTemp[eRefPicList].mvSolidLT[mvpIdx];
        cMvPredSolid[refList][refIdxTemp][1] = affiAMVPInfoTemp[eRefPicList].mvSolidRT[mvpIdx];
        cMvPredSolid[refList][refIdxTemp][2] = affiAMVPInfoTemp[eRefPicList].mvSolidLB[mvpIdx];

        biPDistTempOk = true;
        biPDistTempOk = biPDistTempOk && affiAMVPInfoTemp[eRefPicList].mvSolidLT[mvpIdx] && affiAMVPInfoTemp[eRefPicList].mvSolidRT[mvpIdx];
        biPDistTempOk = biPDistTempOk && ((mvNum > 2) ? affiAMVPInfoTemp[eRefPicList].mvSolidLB[mvpIdx] : true);
      }
#endif

      if (pu.cu->affineType == AFFINEMODEL_6PARAM && refIdx4Para[refList] != refIdxTemp)
      {
        xCopyAffineAMVPInfo(affiAMVPInfoTemp[eRefPicList], aacAffineAMVPInfo[refList][refIdxTemp]);
        continue;
      }

      // set hevc ME result as start search position when it is best than mvp
      for ( int i=0; i<3; i++ )
      {
        mvHevc[i] = hevcMv[refList][refIdxTemp];
        mvHevc[i].roundAffinePrecInternal2Amvr(pu.cu->imv);

#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          mvHevcSolid[i] = hevcMvSolid[refList][refIdxTemp];
        }
#endif
      }
      PelUnitBuf predBuf = m_tmpStorageLCU.getBuf( UnitAreaRelative(*pu.cu, pu) );
#if GDR_ENABLED
      bool uiCandCostOk = true;
      Distortion uiCandCost   = xGetAffineTemplateCost(pu, origBuf, predBuf, mvHevc, aaiMvpIdx[refList][refIdxTemp],
                                                       AMVP_MAX_NUM_CANDS, eRefPicList, refIdxTemp, uiCandCostOk);

      uiCandCostOk = uiCandCostOk && mvHevcSolid[0] && mvHevcSolid[1] && ((mvNum > 2) ? mvHevcSolid[2] : true);

#else
      Distortion uiCandCost = xGetAffineTemplateCost(pu, origBuf, predBuf, mvHevc, aaiMvpIdx[refList][refIdxTemp],
                                                     AMVP_MAX_NUM_CANDS, eRefPicList, refIdxTemp);
#endif

      if ( affineAmvrEnabled )
      {
        uiCandCost += m_pcRdCost->getCost(xCalcAffineMVBits(pu, mvHevc, cMvPred[refList][refIdxTemp]));
      }

      //check stored affine motion
      bool affine4Para    = pu.cu->affineType == AFFINEMODEL_4PARAM;
      bool savedParaAvail =
        pu.cu->imv
        && ((m_affineMotion.affine4ParaRefIdx[refList] == refIdxTemp && affine4Para && m_affineMotion.affine4ParaAvail)
            || (m_affineMotion.affine6ParaRefIdx[refList] == refIdxTemp && !affine4Para
                && m_affineMotion.affine6ParaAvail));

      if ( savedParaAvail )
      {
        Mv mvFour[3];
#if GDR_ENABLED
        bool mvFourSolid[3] = { true, true, true };
#endif
        for ( int i = 0; i < mvNum; i++ )
        {
          mvFour[i] =
            affine4Para ? m_affineMotion.acMvAffine4Para[refList][i] : m_affineMotion.acMvAffine6Para[refList][i];
          mvFour[i].roundAffinePrecInternal2Amvr(pu.cu->imv);
#if GDR_ENABLED
          mvFourSolid[i] = affine4Para ? m_affineMotion.acMvAffine4ParaSolid[refList][i]
                                       : m_affineMotion.acMvAffine6ParaSolid[refList][i];
#endif
        }

#if GDR_ENABLED
        bool candCostInheritOk = true;
        Distortion candCostInherit =
          xGetAffineTemplateCost(pu, origBuf, predBuf, mvFour, aaiMvpIdx[refList][refIdxTemp], AMVP_MAX_NUM_CANDS,
                                 eRefPicList, refIdxTemp, candCostInheritOk);

        candCostInheritOk = candCostInheritOk && mvFourSolid[0] && mvFourSolid[1] && ((mvNum > 2) ? mvFourSolid[2] : true);
#else
        Distortion candCostInherit = xGetAffineTemplateCost(
          pu, origBuf, predBuf, mvFour, aaiMvpIdx[refList][refIdxTemp], AMVP_MAX_NUM_CANDS, eRefPicList, refIdxTemp);
#endif
        candCostInherit += m_pcRdCost->getCost(xCalcAffineMVBits(pu, mvFour, cMvPred[refList][refIdxTemp]));

#if GDR_ENABLED
        allOk = (candCostInherit < uiCandCost);
        if (isEncodeGdrClean)
        {
          if (candCostInheritOk)
          {
            allOk = (uiCandCostOk) ? (candCostInherit < uiCandCost) : true;
          }
          else
          {
            allOk = false;
          }
        }
#endif

#if GDR_ENABLED
        if (allOk)
#else
        if ( candCostInherit < uiCandCost )
#endif
        {
          uiCandCost = candCostInherit;
          memcpy( mvHevc, mvFour, 3 * sizeof( Mv ) );
#if GDR_ENABLED
          if (isEncodeGdrClean)
          {
            uiCandCostOk = candCostInheritOk;
            memcpy(mvHevcSolid, mvFourSolid, 3 * sizeof(bool));
          }
#endif
        }
      }

      if (pu.cu->affineType == AFFINEMODEL_4PARAM && m_affMVListSize
        && (!pu.cu->cs->sps->getUseBcw() || bcwIdx == BCW_DEFAULT)
        )
      {
        int shift = MAX_CU_DEPTH;
        for (int i = 0; i < m_affMVListSize; i++)
        {
          AffineMVInfo *mvInfo = m_affMVList + ((m_affMVListIdx - i - 1 + m_affMVListMaxSize) % (m_affMVListMaxSize));
#if GDR_ENABLED
          AffineMVInfoSolid *mvInfoSolid = m_affMVListSolid + ((m_affMVListIdx - i - 1 + m_affMVListMaxSize) % (m_affMVListMaxSize));
#endif

          //check;
          int j = 0;
          for (; j < i; j++)
          {
            AffineMVInfo *prevMvInfo = m_affMVList + ((m_affMVListIdx - j - 1 + m_affMVListMaxSize) % (m_affMVListMaxSize));
            if ((mvInfo->affMVs[refList][refIdxTemp][0] == prevMvInfo->affMVs[refList][refIdxTemp][0])
                && (mvInfo->affMVs[refList][refIdxTemp][1] == prevMvInfo->affMVs[refList][refIdxTemp][1])
                && (mvInfo->x == prevMvInfo->x) && (mvInfo->y == prevMvInfo->y) && (mvInfo->w == prevMvInfo->w))
            {
              break;
            }
          }
          if (j < i)
          {
            continue;
          }

          Mv mvTmp[3], *nbMv = mvInfo->affMVs[refList][refIdxTemp];
#if GDR_ENABLED
          bool mvTmpSolid[3];
          bool *nbMvSolid = mvInfoSolid->affMVsSolid[refList][refIdxTemp];
          mvTmpSolid[0] = nbMvSolid[0];
          mvTmpSolid[1] = nbMvSolid[1];
#endif
          int vx, vy;
          int dMvHorX, dMvHorY, dMvVerX, dMvVerY;
          int mvScaleHor = nbMv[0].getHor() * (1 << shift);
          int mvScaleVer = nbMv[0].getVer() * (1 << shift);
          Mv dMv = nbMv[1] - nbMv[0];

          dMvHorX = dMv.getHor() * (1 << (shift - floorLog2(mvInfo->w)));
          dMvHorY = dMv.getVer() * (1 << (shift - floorLog2(mvInfo->w)));
          dMvVerX = -dMvHorY;
          dMvVerY = dMvHorX;

          vx = mvScaleHor + dMvHorX * (pu.Y().x - mvInfo->x) + dMvVerX * (pu.Y().y - mvInfo->y);
          vy = mvScaleVer + dMvHorY * (pu.Y().x - mvInfo->x) + dMvVerY * (pu.Y().y - mvInfo->y);
          roundAffineMv(vx, vy, shift);
          mvTmp[0] = Mv(vx, vy);
          mvTmp[0].clipToStorageBitDepth();
          clipMv( mvTmp[0], pu.cu->lumaPos(), pu.cu->lumaSize(), *pu.cs->sps, *pu.cs->pps );
          mvTmp[0].roundAffinePrecInternal2Amvr(pu.cu->imv);
          vx = mvScaleHor + dMvHorX * (pu.Y().x + pu.Y().width - mvInfo->x) + dMvVerX * (pu.Y().y - mvInfo->y);
          vy = mvScaleVer + dMvHorY * (pu.Y().x + pu.Y().width - mvInfo->x) + dMvVerY * (pu.Y().y - mvInfo->y);
          roundAffineMv(vx, vy, shift);
          mvTmp[1] = Mv(vx, vy);
          mvTmp[1].clipToStorageBitDepth();
          clipMv( mvTmp[1], pu.cu->lumaPos(), pu.cu->lumaSize(), *pu.cs->sps, *pu.cs->pps );
          mvTmp[0].roundAffinePrecInternal2Amvr(pu.cu->imv);
          mvTmp[1].roundAffinePrecInternal2Amvr(pu.cu->imv);

#if GDR_ENABLED
          bool tmpCostOk = true;
          Distortion tmpCost   = xGetAffineTemplateCost(pu, origBuf, predBuf, mvTmp, aaiMvpIdx[refList][refIdxTemp],
                                                        AMVP_MAX_NUM_CANDS, eRefPicList, refIdxTemp, tmpCostOk);
          tmpCostOk = tmpCostOk && mvTmpSolid[0] && mvTmpSolid[1];
#else
          Distortion tmpCost = xGetAffineTemplateCost(pu, origBuf, predBuf, mvTmp, aaiMvpIdx[refList][refIdxTemp],
                                                      AMVP_MAX_NUM_CANDS, eRefPicList, refIdxTemp);
#endif
          if ( affineAmvrEnabled )
          {
            tmpCost += m_pcRdCost->getCost(xCalcAffineMVBits(pu, mvTmp, cMvPred[refList][refIdxTemp]));
          }
#if GDR_ENABLED
          allOk = (tmpCost < uiCandCost);
          if (isEncodeGdrClean)
          {
            if (tmpCostOk)
            {
              allOk = (uiCandCostOk) ? (tmpCost < uiCandCost) : true;
            }
            else
            {
              allOk = false;
            }
          }
#endif

#if GDR_ENABLED
          if (allOk)
#else
          if (tmpCost < uiCandCost)
#endif
          {
            uiCandCost = tmpCost;
            std::memcpy(mvHevc, mvTmp, 3 * sizeof(Mv));
#if GDR_ENABLED
            if (isEncodeGdrClean)
            {
              uiCandCostOk = tmpCostOk;
              std::memset(mvHevcSolid, true, 3 * sizeof(bool));
            }
#endif
          }
        }
      }
      if ( pu.cu->affineType == AFFINEMODEL_6PARAM )
      {
        Mv mvFour[3];
        mvFour[0] = mvAffine4Para[refList][refIdxTemp][0];
        mvFour[1] = mvAffine4Para[refList][refIdxTemp][1];
#if GDR_ENABLED
        bool mvFourSolid[3];
        mvFourSolid[0] = mvAffine4ParaSolid[refList][refIdxTemp][0];
        mvFourSolid[1] = mvAffine4ParaSolid[refList][refIdxTemp][1];
#endif

        mvAffine4Para[refList][refIdxTemp][0].roundAffinePrecInternal2Amvr(pu.cu->imv);
        mvAffine4Para[refList][refIdxTemp][1].roundAffinePrecInternal2Amvr(pu.cu->imv);

        int shift = MAX_CU_DEPTH;
        int vx2   = (mvFour[0].getHor() * (1 << shift))
                  - ((mvFour[1].getVer() - mvFour[0].getVer())
                     * (1 << (shift + floorLog2(pu.lheight()) - floorLog2(pu.lwidth()))));
        int vy2 = (mvFour[0].getVer() * (1 << shift))
                  + ((mvFour[1].getHor() - mvFour[0].getHor())
                     * (1 << (shift + floorLog2(pu.lheight()) - floorLog2(pu.lwidth()))));
        int offset = (1 << (shift - 1));
        vx2 = (vx2 + offset - (vx2 >= 0)) >> shift;
        vy2 = (vy2 + offset - (vy2 >= 0)) >> shift;
        mvFour[2].hor = vx2;
        mvFour[2].ver = vy2;
        mvFour[2].clipToStorageBitDepth();
        mvFour[0].roundAffinePrecInternal2Amvr(pu.cu->imv);
        mvFour[1].roundAffinePrecInternal2Amvr(pu.cu->imv);
        mvFour[2].roundAffinePrecInternal2Amvr(pu.cu->imv);

#if GDR_ENABLED
        bool uiCandCostInheritOk = true;
        Distortion uiCandCostInherit =
          xGetAffineTemplateCost(pu, origBuf, predBuf, mvFour, aaiMvpIdx[refList][refIdxTemp], AMVP_MAX_NUM_CANDS,
                                 eRefPicList, refIdxTemp, uiCandCostInheritOk);
        uiCandCostInheritOk = uiCandCostInheritOk && mvFourSolid[0] && mvFourSolid[1];
#else
        Distortion uiCandCostInherit = xGetAffineTemplateCost(
          pu, origBuf, predBuf, mvFour, aaiMvpIdx[refList][refIdxTemp], AMVP_MAX_NUM_CANDS, eRefPicList, refIdxTemp);
#endif

        if ( affineAmvrEnabled )
        {
          uiCandCostInherit += m_pcRdCost->getCost(xCalcAffineMVBits(pu, mvFour, cMvPred[refList][refIdxTemp]));
        }
#if GDR_ENABLED
        allOk = (uiCandCostInherit < uiCandCost);

        if (isEncodeGdrClean)
        {
          if (uiCandCostInheritOk)
          {
            allOk = (uiCandCostOk) ? (uiCandCostInherit < uiCandCost) : true;
          }
          else
          {
            allOk = false;
          }
        }
#endif

#if GDR_ENABLED
        if (allOk)
#else
        if ( uiCandCostInherit < uiCandCost )
#endif
        {
          uiCandCost = uiCandCostInherit;
          for ( int i = 0; i < 3; i++ )
          {
            mvHevc[i] = mvFour[i];
#if GDR_ENABLED
            if (isEncodeGdrClean)
            {
              uiCandCostOk = uiCandCostInheritOk;
              mvHevcSolid[i] = true;
            }
#endif
          }
        }
      }


#if GDR_ENABLED
      allOk = (uiCandCost < biPDistTemp);

      if (isEncodeGdrClean)
      {
        if (uiCandCostOk)
        {
          allOk = (biPDistTempOk) ? (uiCandCost < biPDistTemp) : true;
        }
        else
        {
          allOk = false;
        }
      }
#endif

#if GDR_ENABLED
      if (allOk)
#else
      if ( uiCandCost < biPDistTemp )
#endif
      {
        ::memcpy(cMvTemp[refList][refIdxTemp], mvHevc, sizeof(Mv) * 3);
#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          cMvTempSolid[refList][refIdxTemp][0] = mvHevcSolid[0];
          cMvTempSolid[refList][refIdxTemp][1] = mvHevcSolid[1];
          cMvTempSolid[refList][refIdxTemp][2] = mvHevcSolid[2];
        }
#endif
      }
      else
      {
        ::memcpy(cMvTemp[refList][refIdxTemp], cMvPred[refList][refIdxTemp], sizeof(Mv) * 3);
#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          cMvTempSolid[refList][refIdxTemp][0] = cMvPredSolid[refList][refIdxTemp][0];
          cMvTempSolid[refList][refIdxTemp][1] = cMvPredSolid[refList][refIdxTemp][1];
          cMvTempSolid[refList][refIdxTemp][2] = cMvPredSolid[refList][refIdxTemp][2];
        }
#endif
      }

      // GPB list 1, save the best MvpIdx, RefIdx and Cost
#if GDR_ENABLED
      allOk = (slice.getPicHeader()->getMvdL1ZeroFlag() && refList == 1 && (biPDistTemp < bestBiPDist));

      if (isEncodeGdrClean)
      {
        if (biPDistTempOk)
        {
          allOk = (bestBiPDistOk)
                    ? (slice.getPicHeader()->getMvdL1ZeroFlag() && refList == 1 && (biPDistTemp < bestBiPDist))
                    : true;
        }
        else
        {
          allOk = false;
        }
      }
#endif

#if GDR_ENABLED
      if (allOk)
#else
      if (slice.getPicHeader()->getMvdL1ZeroFlag() && refList == 1 && biPDistTemp < bestBiPDist)
#endif
      {
        bestBiPDist = biPDistTemp;
        bestBiPMvpL1    = aaiMvpIdx[refList][refIdxTemp];
        bestBiPRefIdxL1 = refIdxTemp;
#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          bestBiPDistOk = biPDistTempOk;
        }
#endif
      }

      // Update bits
      bitsTemp += m_auiMVPIdxCost[aaiMvpIdx[refList][refIdxTemp]][AMVP_MAX_NUM_CANDS];

      if (m_pcEncCfg->getFastMEForGenBLowDelayEnabled() && refList == 1)   // list 1
      {
        if (slice.getList1IdxToList0Idx(refIdxTemp) >= 0
            && (pu.cu->affineType != AFFINEMODEL_6PARAM || slice.getList1IdxToList0Idx(refIdxTemp) == refIdx4Para[0]))
        {
          int iList1ToList0Idx = slice.getList1IdxToList0Idx(refIdxTemp);
          ::memcpy(cMvTemp[1][refIdxTemp], cMvTemp[0][iList1ToList0Idx], sizeof(Mv) * 3);
#if GDR_ENABLED
          if (isEncodeGdrClean)
          {
            ::memcpy(cMvTempSolid[1][refIdxTemp], cMvTempSolid[0][iList1ToList0Idx], sizeof(bool) * 3);
            uiCostTempOk = uiCostTempL0Ok[iList1ToList0Idx];
          }
#endif
          costTemp = uiCostTempL0[iList1ToList0Idx];

          costTemp -= m_pcRdCost->getCost(uiBitsTempL0[iList1ToList0Idx]);
          bitsTemp += xCalcAffineMVBits(pu, cMvTemp[refList][refIdxTemp], cMvPred[refList][refIdxTemp]);
          /*calculate the correct cost*/
          costTemp += m_pcRdCost->getCost(bitsTemp);
          DTRACE(g_trace_ctx, D_COMMON, " (%d) costTemp=%d\n", DTRACE_GET_COUNTER(g_trace_ctx, D_COMMON), costTemp);
        }
        else
        {

//RAMIRO: chama a funcao AffineMotionEstimation
#if GDR_ENABLED
          xAffineMotionEstimation(pu, origBuf, eRefPicList, cMvPred[refList][refIdxTemp], refIdxTemp,
                                  cMvTemp[refList][refIdxTemp], cMvTempSolid[refList][refIdxTemp], bitsTemp, costTemp,
                                  aaiMvpIdx[refList][refIdxTemp], affiAMVPInfoTemp[eRefPicList], bAnyClean
#else
          xAffineMotionEstimation(pu, origBuf, eRefPicList, cMvPred[refList][refIdxTemp], refIdxTemp,
                                  cMvTemp[refList][refIdxTemp], bitsTemp, costTemp, aaiMvpIdx[refList][refIdxTemp],
                                  affiAMVPInfoTemp[eRefPicList]
#endif
          );

#if GDR_ENABLED
          if (isEncodeGdrClean)
          {
            int            mvpIdx = aaiMvpIdx[refList][refIdxTemp];
            PelUnitBuf     tmpBuf = m_tmpAffiStorage.getBuf(UnitAreaRelative(*pu.cu, pu));
            const Picture *refPic = pu.cu->slice->getRefPic((RefPicList) refList, refIdxTemp);

            cMvPredSolid[refList][refIdxTemp][0] = affiAMVPInfoTemp[eRefPicList].mvSolidLT[mvpIdx];
            cMvPredSolid[refList][refIdxTemp][1] = affiAMVPInfoTemp[eRefPicList].mvSolidRT[mvpIdx];
            cMvPredSolid[refList][refIdxTemp][2] = affiAMVPInfoTemp[eRefPicList].mvSolidLB[mvpIdx];

            cMvTempSolid[refList][refIdxTemp][0] = cMvPredSolid[refList][refIdxTemp][0];
            cMvTempSolid[refList][refIdxTemp][1] = cMvPredSolid[refList][refIdxTemp][1];
            cMvTempSolid[refList][refIdxTemp][2] = cMvPredSolid[refList][refIdxTemp][2];

            bool isSubPuYYClean = xPredAffineBlk(COMPONENT_Y, pu, refPic, cMvTemp[refList][refIdxTemp], tmpBuf, false,
                                                 pu.cu->slice->clpRng(COMPONENT_Y));
            bool isSubPuCbClean = (isSubPuYYClean)
                                    ? xPredAffineBlk(COMPONENT_Cb, pu, refPic, cMvTemp[refList][refIdxTemp], tmpBuf,
                                                     false, pu.cu->slice->clpRng(COMPONENT_Cb))
                                    : false;

            cMvTempValid[refList][refIdxTemp][0] = isSubPuYYClean && isSubPuCbClean;
            cMvTempValid[refList][refIdxTemp][1] = isSubPuYYClean && isSubPuCbClean;
            cMvTempValid[refList][refIdxTemp][2] = isSubPuYYClean && isSubPuCbClean;

            uiCostTempOk = true;
            uiCostTempOk = uiCostTempOk && cMvPredSolid[refList][refIdxTemp][0] && cMvPredSolid[refList][refIdxTemp][1];
            uiCostTempOk = uiCostTempOk && ((mvNum > 2) ? cMvPredSolid[refList][refIdxTemp][2] : true);
            uiCostTempOk = uiCostTempOk && cMvTempSolid[refList][refIdxTemp][0] && cMvTempSolid[refList][refIdxTemp][1]
                           && ((mvNum > 2) ? cMvTempSolid[refList][refIdxTemp][2] : true);
            uiCostTempOk = uiCostTempOk && cMvTempValid[refList][refIdxTemp][0] && cMvTempValid[refList][refIdxTemp][1]
                           && ((mvNum > 2) ? cMvTempValid[refList][refIdxTemp][2] : true);
          }
#endif
        }
      }
      else
      {

//RAMIRO: chama a funcao AffineMotionEstimation
#if GDR_ENABLED
        xAffineMotionEstimation(pu, origBuf, eRefPicList, cMvPred[refList][refIdxTemp], refIdxTemp,
                                cMvTemp[refList][refIdxTemp], cMvTempSolid[refList][refIdxTemp], bitsTemp, costTemp,
                                aaiMvpIdx[refList][refIdxTemp], affiAMVPInfoTemp[eRefPicList], bAnyClean);
#else
        xAffineMotionEstimation(pu, origBuf, eRefPicList, cMvPred[refList][refIdxTemp], refIdxTemp,
                                cMvTemp[refList][refIdxTemp], bitsTemp, costTemp, aaiMvpIdx[refList][refIdxTemp],
                                affiAMVPInfoTemp[eRefPicList]);
#endif

#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          int            mvpIdx = aaiMvpIdx[refList][refIdxTemp];
          PelUnitBuf     tmpBuf = m_tmpAffiStorage.getBuf(UnitAreaRelative(*pu.cu, pu));
          const Picture *refPic = pu.cu->slice->getRefPic((RefPicList) refList, refIdxTemp);

          cMvPredSolid[refList][refIdxTemp][0] = affiAMVPInfoTemp[eRefPicList].mvSolidLT[mvpIdx];
          cMvPredSolid[refList][refIdxTemp][1] = affiAMVPInfoTemp[eRefPicList].mvSolidRT[mvpIdx];
          cMvPredSolid[refList][refIdxTemp][2] = affiAMVPInfoTemp[eRefPicList].mvSolidLB[mvpIdx];

          cMvTempSolid[refList][refIdxTemp][0] = cMvPredSolid[refList][refIdxTemp][0];
          cMvTempSolid[refList][refIdxTemp][1] = cMvPredSolid[refList][refIdxTemp][1];
          cMvTempSolid[refList][refIdxTemp][2] = cMvPredSolid[refList][refIdxTemp][2];

          bool isSubPuYYClean = xPredAffineBlk(COMPONENT_Y, pu, refPic, cMvTemp[refList][refIdxTemp], tmpBuf, false,
                                               pu.cu->slice->clpRng(COMPONENT_Y));
          bool isSubPuCbClean = (isSubPuYYClean)
                                  ? xPredAffineBlk(COMPONENT_Cb, pu, refPic, cMvTemp[refList][refIdxTemp], tmpBuf,
                                                   false, pu.cu->slice->clpRng(COMPONENT_Cb))
                                  : false;

          cMvTempValid[refList][refIdxTemp][0] = isSubPuYYClean && isSubPuCbClean;
          cMvTempValid[refList][refIdxTemp][1] = isSubPuYYClean && isSubPuCbClean;
          cMvTempValid[refList][refIdxTemp][2] = isSubPuYYClean && isSubPuCbClean;

          uiCostTempOk = true;
          uiCostTempOk = uiCostTempOk && cMvPredSolid[refList][refIdxTemp][0] && cMvPredSolid[refList][refIdxTemp][1];
          uiCostTempOk = uiCostTempOk && ((mvNum > 2) ? cMvPredSolid[refList][refIdxTemp][2] : true);
          uiCostTempOk = uiCostTempOk && cMvTempSolid[refList][refIdxTemp][0] && cMvTempSolid[refList][refIdxTemp][1]
                         && ((mvNum > 2) ? cMvTempSolid[refList][refIdxTemp][2] : true);
          uiCostTempOk = uiCostTempOk && cMvTempValid[refList][refIdxTemp][0] && cMvTempValid[refList][refIdxTemp][1]
                         && ((mvNum > 2) ? cMvTempValid[refList][refIdxTemp][2] : true);
        }
#endif
      }
      if(pu.cu->cs->sps->getUseBcw() && pu.cu->BcwIdx == BCW_DEFAULT && pu.cu->slice->isInterB())
      {
        m_uniMotions.setReadModeAffine(true, (uint8_t) refList, (uint8_t) refIdxTemp, pu.cu->affineType);
#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          m_uniMotions.copyAffineMvFrom(cMvTemp[refList][refIdxTemp], cMvTempSolid[refList][refIdxTemp],
                                        costTemp - m_pcRdCost->getCost(bitsTemp), (uint8_t) refList,
                                        (uint8_t) refIdxTemp, pu.cu->affineType, aaiMvpIdx[refList][refIdxTemp]);
        }
        else
        {
          m_uniMotions.copyAffineMvFrom(cMvTemp[refList][refIdxTemp], costTemp - m_pcRdCost->getCost(bitsTemp),
                                        (uint8_t) refList, (uint8_t) refIdxTemp, pu.cu->affineType,
                                        aaiMvpIdx[refList][refIdxTemp]);
        }
#else
        m_uniMotions.copyAffineMvFrom(cMvTemp[refList][refIdxTemp], costTemp - m_pcRdCost->getCost(bitsTemp),
                                      (uint8_t) refList, (uint8_t) refIdxTemp, pu.cu->affineType,
                                      aaiMvpIdx[refList][refIdxTemp]);
#endif
      }
      // Set best AMVP Index
      xCopyAffineAMVPInfo(affiAMVPInfoTemp[eRefPicList], aacAffineAMVPInfo[refList][refIdxTemp]);
#if GDR_ENABLED
      if ( pu.cu->imv != 2 || !m_pcEncCfg->getUseAffineAmvrEncOpt() )
      {
        xCheckBestAffineMVP(pu, affiAMVPInfoTemp[eRefPicList], eRefPicList, cMvTemp[refList][refIdxTemp],
                            cMvPred[refList][refIdxTemp], aaiMvpIdx[refList][refIdxTemp], bitsTemp, costTemp);
        if (isEncodeGdrClean)
        {
          int mvpIdx = aaiMvpIdx[refList][refIdxTemp];

          cMvPredSolid[refList][refIdxTemp][0] = affiAMVPInfoTemp[eRefPicList].mvSolidLT[mvpIdx];
          cMvPredSolid[refList][refIdxTemp][1] = affiAMVPInfoTemp[eRefPicList].mvSolidRT[mvpIdx];
          cMvPredSolid[refList][refIdxTemp][2] = affiAMVPInfoTemp[eRefPicList].mvSolidLB[mvpIdx];

          cMvTempSolid[refList][refIdxTemp][0] = cMvPredSolid[refList][refIdxTemp][0];
          cMvTempSolid[refList][refIdxTemp][1] = cMvPredSolid[refList][refIdxTemp][0];
          cMvTempSolid[refList][refIdxTemp][2] = cMvPredSolid[refList][refIdxTemp][0];

          if (cMvTempValid[refList][refIdxTemp][0] && cMvTempValid[refList][refIdxTemp][1]
              && cMvTempValid[refList][refIdxTemp][2])
          {
            cMvTempValid[refList][refIdxTemp][0] = cMvPredSolid[refList][refIdxTemp][0];
            cMvTempValid[refList][refIdxTemp][1] = cMvPredSolid[refList][refIdxTemp][0];
            cMvTempValid[refList][refIdxTemp][2] = cMvPredSolid[refList][refIdxTemp][0];
          }

          uiCostTempOk = true;
          uiCostTempOk = uiCostTempOk && cMvPredSolid[refList][refIdxTemp][0] && cMvPredSolid[refList][refIdxTemp][1];
          uiCostTempOk = uiCostTempOk && ((mvNum > 2) ? cMvPredSolid[refList][refIdxTemp][2] : true);
          uiCostTempOk = uiCostTempOk && cMvTempSolid[refList][refIdxTemp][0] && cMvTempSolid[refList][refIdxTemp][1]
                         && ((mvNum > 2) ? cMvTempSolid[refList][refIdxTemp][2] : true);
          uiCostTempOk = uiCostTempOk && cMvTempValid[refList][refIdxTemp][0] && cMvTempValid[refList][refIdxTemp][1]
                         && ((mvNum > 2) ? cMvTempValid[refList][refIdxTemp][2] : true);
        }
      }
#else
      if ( pu.cu->imv != 2 || !m_pcEncCfg->getUseAffineAmvrEncOpt() )
      {
        xCheckBestAffineMVP(pu, affiAMVPInfoTemp[eRefPicList], eRefPicList, cMvTemp[refList][refIdxTemp],
                            cMvPred[refList][refIdxTemp], aaiMvpIdx[refList][refIdxTemp], bitsTemp, costTemp);
      }
#endif

      if (refList == 0)
      {
        uiCostTempL0[refIdxTemp] = costTemp;
        uiBitsTempL0[refIdxTemp] = bitsTemp;
#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          uiCostTempL0Ok[refIdxTemp] = uiCostTempOk;
        }
#endif
      }
      DTRACE(g_trace_ctx, D_COMMON, " (%d) costTemp=%d, uiCost[refList]=%d\n",
             DTRACE_GET_COUNTER(g_trace_ctx, D_COMMON), costTemp, uiCost[refList]);
#if GDR_ENABLED
      allOk = (costTemp < uiCost[refList]);

      if (isEncodeGdrClean)
      {
        if (uiCostTempOk)
        {
          allOk = (uiCostOk[refList]) ? (costTemp < uiCost[refList]) : true;
        }
        else
        {
          allOk = false;
        }
      }
#endif

#if GDR_ENABLED
      if (allOk)
#else
      if (costTemp < uiCost[refList])
#endif
      {
        uiCost[refList] = costTemp;
        bits[refList]   = bitsTemp;   // storing for bi-prediction

#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          uiCostOk[refList] = uiCostTempOk;
        }
#endif
        // set best motion
        ::memcpy(aacMv[refList], cMvTemp[refList][refIdxTemp], sizeof(Mv) * 3);
#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          ::memcpy(aacMvSolid[refList], cMvTempSolid[refList][refIdxTemp], sizeof(bool) * 3);
          ::memcpy(aacMvValid[refList], cMvTempValid[refList][refIdxTemp], sizeof(bool) * 3);
        }
#endif
        refIdx[refList] = refIdxTemp;
      }


#if GDR_ENABLED
      allOk = (refList == 1 && costTemp < costValidList1 && slice.getList1IdxToList0Idx(refIdxTemp) < 0);

      if (isEncodeGdrClean)
      {
        if (uiCostTempOk)
        {
          allOk = (costValidList1Ok)
                    ? (refList == 1 && costTemp < costValidList1 && slice.getList1IdxToList0Idx(refIdxTemp) < 0)
                    : true;
        }
        else
        {
          allOk = false;
        }
      }
#endif


#if GDR_ENABLED
      if (allOk)
#else
      if (refList == 1 && costTemp < costValidList1 && slice.getList1IdxToList0Idx(refIdxTemp) < 0)
#endif
      {
        costValidList1 = costTemp;
        bitsValidList1 = bitsTemp;

        // set motion
        memcpy(mvValidList1, cMvTemp[refList][refIdxTemp], sizeof(Mv) * 3);

#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          costValidList1Ok = uiCostTempOk;
          ::memcpy(mvValidList1Solid, cMvTempSolid[refList][refIdxTemp], sizeof(bool) * 3);
          ::memcpy(mvValidList1Valid, cMvTempSolid[refList][refIdxTemp], sizeof(bool) * 3);
        }
#endif
        refIdxValidList1 = refIdxTemp;
      }
    } // End refIdx loop
  } // end Uni-prediction

  if ( pu.cu->affineType == AFFINEMODEL_4PARAM )
  {
    ::memcpy( mvAffine4Para, cMvTemp, sizeof( cMvTemp ) );
#if GDR_ENABLED
    ::memcpy(mvAffine4ParaSolid, cMvTempSolid, sizeof(cMvTempSolid));
#endif
    if ( pu.cu->imv == 0 && ( !pu.cu->cs->sps->getUseBcw() || bcwIdx == BCW_DEFAULT ) )
    {
      AffineMVInfo *affMVInfo = m_affMVList + m_affMVListIdx;
#if GDR_ENABLED
      AffineMVInfoSolid *affMVInfoSolid = m_affMVListSolid + m_affMVListIdx;
#endif

      //check;
      int j = 0;
      for (; j < m_affMVListSize; j++)
      {
        AffineMVInfo *prevMvInfo = m_affMVList + ((m_affMVListIdx - j - 1 + m_affMVListMaxSize) % (m_affMVListMaxSize));
        if ((pu.Y().x == prevMvInfo->x) && (pu.Y().y == prevMvInfo->y) && (pu.Y().width == prevMvInfo->w) && (pu.Y().height == prevMvInfo->h))
        {
          break;
        }
      }
#if GDR_ENABLED
      if (j < m_affMVListSize)
      {
        affMVInfo = m_affMVList + ((m_affMVListIdx - j - 1 + m_affMVListMaxSize) % (m_affMVListMaxSize));
        affMVInfoSolid = m_affMVListSolid + ((m_affMVListIdx - j - 1 + m_affMVListMaxSize) % (m_affMVListMaxSize));
      }
      ::memcpy(affMVInfo->affMVs, cMvTemp, sizeof(cMvTemp));
      ::memcpy(affMVInfoSolid->affMVsSolid, cMvTempSolid, sizeof(cMvTempSolid));
#else
      if (j < m_affMVListSize)
        affMVInfo = m_affMVList + ((m_affMVListIdx - j - 1 + m_affMVListMaxSize) % (m_affMVListMaxSize));
      ::memcpy(affMVInfo->affMVs, cMvTemp, sizeof(cMvTemp));
#endif

      if (j == m_affMVListSize)
      {
        affMVInfo->x = pu.Y().x;
        affMVInfo->y = pu.Y().y;
        affMVInfo->w = pu.Y().width;
        affMVInfo->h = pu.Y().height;
        m_affMVListSize = std::min(m_affMVListSize + 1, m_affMVListMaxSize);
        m_affMVListIdx = (m_affMVListIdx + 1) % (m_affMVListMaxSize);
      }
    }
  }

  // Bi-directional prediction
  if ( slice.isInterB() && !PU::isBipredRestriction(pu) )
  {
    tryBipred = 1;
    pu.interDir = 3;
    m_isBi = true;
    // Set as best list0 and list1
    iRefIdxBi[0] = refIdx[0];
    iRefIdxBi[1] = refIdx[1];

    ::memcpy( cMvBi,       aacMv,     sizeof(aacMv)     );
    ::memcpy( cMvPredBi,   cMvPred,   sizeof(cMvPred)   );
    ::memcpy( aaiMvpIdxBi, aaiMvpIdx, sizeof(aaiMvpIdx) );

#if GDR_ENABLED
    if (isEncodeGdrClean)
    {
      ::memcpy(cMvBiSolid, aacMvSolid, sizeof(cMvBiSolid));
      ::memcpy(cMvBiValid, aacMvValid, sizeof(cMvBiValid));
      ::memcpy(cMvPredBiSolid, cMvPredSolid, sizeof(cMvPredSolid));
    }
#endif

    uint32_t motBits[2];
    bool doBiPred = true;

    if ( slice.getPicHeader()->getMvdL1ZeroFlag() ) // GPB, list 1 only use Mvp
    {
      xCopyAffineAMVPInfo( aacAffineAMVPInfo[1][bestBiPRefIdxL1], affiAMVPInfoTemp[REF_PIC_LIST_1] );
      pu.mvpIdx[REF_PIC_LIST_1] = bestBiPMvpL1;
      aaiMvpIdxBi[1][bestBiPRefIdxL1] = bestBiPMvpL1;

      // Set Mv for list1
      Mv pcMvTemp[3] = { affiAMVPInfoTemp[REF_PIC_LIST_1].mvCandLT[bestBiPMvpL1],
                         affiAMVPInfoTemp[REF_PIC_LIST_1].mvCandRT[bestBiPMvpL1],
                         affiAMVPInfoTemp[REF_PIC_LIST_1].mvCandLB[bestBiPMvpL1] };
      ::memcpy( cMvPredBi[1][bestBiPRefIdxL1], pcMvTemp, sizeof(Mv)*3 );
      ::memcpy( cMvBi[1],                      pcMvTemp, sizeof(Mv)*3 );
      ::memcpy( cMvTemp[1][bestBiPRefIdxL1],   pcMvTemp, sizeof(Mv)*3 );
      iRefIdxBi[1] = bestBiPRefIdxL1;
#if GDR_ENABLED
      if (isEncodeGdrClean)
      {
        PelUnitBuf     tmpBuf = m_tmpAffiStorage.getBuf(UnitAreaRelative(*pu.cu, pu));
        const Picture *refPic = pu.cu->slice->getRefPic((RefPicList)REF_PIC_LIST_1, iRefIdxBi[1]);

        cMvPredBiSolid[1][bestBiPRefIdxL1][0] = affiAMVPInfoTemp[REF_PIC_LIST_1].mvSolidLT[bestBiPMvpL1];
        cMvPredBiSolid[1][bestBiPRefIdxL1][1] = affiAMVPInfoTemp[REF_PIC_LIST_1].mvSolidRT[bestBiPMvpL1];
        cMvPredBiSolid[1][bestBiPRefIdxL1][2] = affiAMVPInfoTemp[REF_PIC_LIST_1].mvSolidLB[bestBiPMvpL1];

        cMvBiSolid[1][0] = affiAMVPInfoTemp[REF_PIC_LIST_1].mvSolidLT[bestBiPMvpL1];
        cMvBiSolid[1][1] = affiAMVPInfoTemp[REF_PIC_LIST_1].mvSolidRT[bestBiPMvpL1];
        cMvBiSolid[1][2] = affiAMVPInfoTemp[REF_PIC_LIST_1].mvSolidLB[bestBiPMvpL1];


        bool isSubPuYYClean = xPredAffineBlk(COMPONENT_Y, pu, refPic, cMvTemp[1][bestBiPRefIdxL1], tmpBuf, false, pu.cu->slice->clpRng(COMPONENT_Y));
        bool isSubPuCbClean = (isSubPuYYClean) ? xPredAffineBlk(COMPONENT_Cb, pu, refPic, cMvTemp[1][bestBiPRefIdxL1], tmpBuf, false, pu.cu->slice->clpRng(COMPONENT_Cb)) : false;

        cMvBiValid[1][0] = isSubPuYYClean && isSubPuCbClean;
        cMvBiValid[1][1] = isSubPuYYClean && isSubPuCbClean;
        cMvBiValid[1][2] = isSubPuYYClean && isSubPuCbClean;

        cMvTempSolid[1][bestBiPRefIdxL1][0] = affiAMVPInfoTemp[REF_PIC_LIST_1].mvSolidLT[bestBiPMvpL1];
        cMvTempSolid[1][bestBiPRefIdxL1][1] = affiAMVPInfoTemp[REF_PIC_LIST_1].mvSolidRT[bestBiPMvpL1];
        cMvTempSolid[1][bestBiPRefIdxL1][2] = affiAMVPInfoTemp[REF_PIC_LIST_1].mvSolidLB[bestBiPMvpL1];
      }
#endif

      if( m_pcEncCfg->getMCTSEncConstraint() )
      {
        Area curTileAreaRestricted;
        curTileAreaRestricted = pu.cs->picture->mctsInfo.getTileAreaSubPelRestricted( pu );
        for( int i = 0; i < mvNum; i++ )
        {
          Mv restrictedMv = pcMvTemp[i];
          MCTSHelper::clipMvToArea( restrictedMv, pu.cu->Y(), curTileAreaRestricted, *pu.cs->sps );

          // If sub-pel filter samples are not inside of allowed area
          if( restrictedMv != pcMvTemp[i] )
          {
            uiCostBi = std::numeric_limits<Distortion>::max();
            doBiPred = false;
          }
        }
      }
      // Get list1 prediction block
      PU::setAllAffineMv( pu, cMvBi[1][0], cMvBi[1][1], cMvBi[1][2], REF_PIC_LIST_1);
      pu.refIdx[REF_PIC_LIST_1] = iRefIdxBi[1];

#if GDR_ENABLED
      if (isEncodeGdrClean)
      {
        PelUnitBuf     tmpBuf = m_tmpAffiStorage.getBuf(UnitAreaRelative(*pu.cu, pu));
        const Picture *refPic = pu.cu->slice->getRefPic((RefPicList)REF_PIC_LIST_1, pu.refIdx[REF_PIC_LIST_1]);

        pu.mvAffiSolid[REF_PIC_LIST_1][0] = cMvBiSolid[REF_PIC_LIST_1][0];
        pu.mvAffiSolid[REF_PIC_LIST_1][1] = cMvBiSolid[REF_PIC_LIST_1][1];
        pu.mvAffiSolid[REF_PIC_LIST_1][2] = cMvBiSolid[REF_PIC_LIST_1][2];


        bool isSubPuYYClean = xPredAffineBlk(COMPONENT_Y, pu, refPic, cMvBi[1], tmpBuf, false, pu.cu->slice->clpRng(COMPONENT_Y));
        bool isSubPuCbClean = (isSubPuYYClean) ? xPredAffineBlk(COMPONENT_Cb, pu, refPic, cMvBi[1], tmpBuf, false, pu.cu->slice->clpRng(COMPONENT_Cb)) : false;

        pu.mvAffiValid[REF_PIC_LIST_1][0] = cMvBiValid[REF_PIC_LIST_1][0] = isSubPuYYClean && isSubPuCbClean;
        pu.mvAffiValid[REF_PIC_LIST_1][1] = cMvBiValid[REF_PIC_LIST_1][1] = isSubPuYYClean && isSubPuCbClean;
        pu.mvAffiValid[REF_PIC_LIST_1][2] = cMvBiValid[REF_PIC_LIST_1][2] = isSubPuYYClean && isSubPuCbClean;
      }
#endif

      PelUnitBuf predBufTmp = m_tmpPredStorage[REF_PIC_LIST_1].getBuf( UnitAreaRelative(*pu.cu, pu) );
      motionCompensation( pu, predBufTmp, REF_PIC_LIST_1 );

      // Update bits
      motBits[0] = bits[0] - mbBits[0];
      motBits[1] = mbBits[1];

      if( slice.getNumRefIdx(REF_PIC_LIST_1) > 1 )
      {
        motBits[1] += bestBiPRefIdxL1 + 1;
        if( bestBiPRefIdxL1 == slice.getNumRefIdx(REF_PIC_LIST_1)-1 )
        {
          motBits[1]--;
        }
      }
      motBits[1] += m_auiMVPIdxCost[aaiMvpIdxBi[1][bestBiPRefIdxL1]][AMVP_MAX_NUM_CANDS];
      bits[2] = mbBits[2] + motBits[0] + motBits[1];
    }
    else
    {
      motBits[0] = bits[0] - mbBits[0];
      motBits[1] = bits[1] - mbBits[1];
      bits[2]    = mbBits[2] + motBits[0] + motBits[1];
    }

    if( doBiPred )
    {
    // 4-times iteration (default)
    int numIter = 4;
    // fast encoder setting or GPB: only one iteration
    if ( m_pcEncCfg->getFastInterSearchMode()==FASTINTERSEARCH_MODE1 || m_pcEncCfg->getFastInterSearchMode()==FASTINTERSEARCH_MODE2 || slice.getPicHeader()->getMvdL1ZeroFlag() )
    {
      numIter = 1;
    }

    for (int iter = 0; iter < numIter; iter++)
    {
      // Set RefList
      int refList = iter % 2;
      if ( m_pcEncCfg->getFastInterSearchMode()==FASTINTERSEARCH_MODE1 || m_pcEncCfg->getFastInterSearchMode()==FASTINTERSEARCH_MODE2 )
      {
#if GDR_ENABLED
        allOk = (uiCost[0] <= uiCost[1]);

        if (isEncodeGdrClean)
        {
          if (uiCostOk[0])
          {
            allOk = (uiCostOk[1]) ? (uiCost[0] <= uiCost[1]) : true;
          }
          else
          {
            allOk = false;
          }
        }
#endif

#if GDR_ENABLED
        if (allOk)
#else
        if( uiCost[0] <= uiCost[1] )
#endif
        {
          refList = 1;
        }
        else
        {
          refList = 0;
        }
        if( bcwIdx != BCW_DEFAULT )
        {
          refList = (abs(getBcwWeight(bcwIdx, REF_PIC_LIST_0)) > abs(getBcwWeight(bcwIdx, REF_PIC_LIST_1)) ? 1 : 0);
        }
      }
      else if (iter == 0)
      {
        refList = 0;
      }

      // First iterate, get prediction block of opposite direction
      if (iter == 0 && !slice.getPicHeader()->getMvdL1ZeroFlag())
      {
        PU::setAllAffineMv(pu, aacMv[1 - refList][0], aacMv[1 - refList][1], aacMv[1 - refList][2],
                           RefPicList(1 - refList));
        pu.refIdx[1 - refList] = refIdx[1 - refList];
#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          PelUnitBuf     tmpBuf = m_tmpAffiStorage.getBuf(UnitAreaRelative(*pu.cu, pu));
          const Picture *refPic = pu.cu->slice->getRefPic((RefPicList) (1 - refList), pu.refIdx[1 - refList]);

          pu.mvAffiSolid[1 - refList][0] = aacMvSolid[1 - refList][0];
          pu.mvAffiSolid[1 - refList][1] = aacMvSolid[1 - refList][1];
          pu.mvAffiSolid[1 - refList][2] = aacMvSolid[1 - refList][2];

          bool isSubPuYYClean = xPredAffineBlk(COMPONENT_Y, pu, refPic, aacMv[1 - refList], tmpBuf, false,
                                               pu.cu->slice->clpRng(COMPONENT_Y));
          bool isSubPuCbClean = (isSubPuYYClean) ? xPredAffineBlk(COMPONENT_Cb, pu, refPic, aacMv[1 - refList], tmpBuf,
                                                                  false, pu.cu->slice->clpRng(COMPONENT_Cb))
                                                 : false;

          pu.mvAffiValid[1 - refList][0] = aacMvValid[1 - refList][0] = isSubPuYYClean && isSubPuCbClean;
          pu.mvAffiValid[1 - refList][1] = aacMvValid[1 - refList][1] = isSubPuYYClean && isSubPuCbClean;
          pu.mvAffiValid[1 - refList][2] = aacMvValid[1 - refList][2] = isSubPuYYClean && isSubPuCbClean;
        }
#endif

        PelUnitBuf predBufTmp = m_tmpPredStorage[1 - refList].getBuf(UnitAreaRelative(*pu.cu, pu));
        motionCompensation(pu, predBufTmp, RefPicList(1 - refList));
      }

      RefPicList eRefPicList = (refList ? REF_PIC_LIST_1 : REF_PIC_LIST_0);

      if ( slice.getPicHeader()->getMvdL1ZeroFlag() ) // GPB, fix List 1, search List 0
      {
        refList     = 0;
        eRefPicList = REF_PIC_LIST_0;
      }

      bool changed = false;

      iRefStart = 0;
      iRefEnd   = slice.getNumRefIdx(eRefPicList) - 1;
      for (int refIdxTemp = iRefStart; refIdxTemp <= iRefEnd; refIdxTemp++)
      {
        if (pu.cu->affineType == AFFINEMODEL_6PARAM && refIdx4Para[refList] != refIdxTemp)
        {
          continue;
        }
        if (m_pcEncCfg->getUseBcwFast() && (bcwIdx != BCW_DEFAULT)
            && (pu.cu->slice->getRefPic(eRefPicList, refIdxTemp)->getPOC()
                == pu.cu->slice->getRefPic(RefPicList(1 - refList), pu.refIdx[1 - refList])->getPOC())
            && (pu.cu->affineType == AFFINEMODEL_4PARAM && pu.cu->slice->getTLayer() > 1))
        {
          continue;
        }
        // update bits
        bitsTemp = mbBits[2] + motBits[1 - refList];
        bitsTemp += ((pu.cu->slice->getSPS()->getUseBcw() == true) ? bcwIdxBits : 0);
        if( slice.getNumRefIdx(eRefPicList) > 1 )
        {
          bitsTemp += refIdxTemp + 1;
          if (refIdxTemp == slice.getNumRefIdx(eRefPicList) - 1)
          {
            bitsTemp--;
          }
        }
        bitsTemp += m_auiMVPIdxCost[aaiMvpIdxBi[refList][refIdxTemp]][AMVP_MAX_NUM_CANDS];

        // call Affine ME

//RAMIRO: chama a funcao AffineMotionEstimation
#if GDR_ENABLED
        xAffineMotionEstimation(pu, origBuf, eRefPicList, cMvPredBi[refList][refIdxTemp], refIdxTemp,
                                cMvTemp[refList][refIdxTemp], cMvTempSolid[refList][refIdxTemp], bitsTemp, costTemp,
                                aaiMvpIdxBi[refList][refIdxTemp], aacAffineAMVPInfo[refList][refIdxTemp], bAnyClean,
                                true);
#else
        xAffineMotionEstimation(pu, origBuf, eRefPicList, cMvPredBi[refList][refIdxTemp], refIdxTemp,
                                cMvTemp[refList][refIdxTemp], bitsTemp, costTemp, aaiMvpIdxBi[refList][refIdxTemp],
                                aacAffineAMVPInfo[refList][refIdxTemp], true);
#endif

#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          int            mvpIdx = aaiMvpIdx[refList][refIdxTemp];
          PelUnitBuf     tmpBuf = m_tmpAffiStorage.getBuf(UnitAreaRelative(*pu.cu, pu));
          const Picture *refPic = pu.cu->slice->getRefPic((RefPicList) refList, refIdxTemp);

          cMvPredBiSolid[refList][refIdxTemp][0] = aacAffineAMVPInfo[refList][refIdxTemp].mvSolidLT[mvpIdx];
          cMvPredBiSolid[refList][refIdxTemp][1] = aacAffineAMVPInfo[refList][refIdxTemp].mvSolidRT[mvpIdx];
          cMvPredBiSolid[refList][refIdxTemp][2] = aacAffineAMVPInfo[refList][refIdxTemp].mvSolidLB[mvpIdx];

          cMvTempSolid[refList][refIdxTemp][0] = cMvPredBiSolid[refList][refIdxTemp][0];
          cMvTempSolid[refList][refIdxTemp][1] = cMvPredSolid[refList][refIdxTemp][1];
          cMvTempSolid[refList][refIdxTemp][2] = cMvPredSolid[refList][refIdxTemp][2];

          bool isSubPuYYClean = xPredAffineBlk(COMPONENT_Y, pu, refPic, cMvTemp[refList][refIdxTemp], tmpBuf, false,
                                               pu.cu->slice->clpRng(COMPONENT_Y));
          bool isSubPuCbClean = (isSubPuYYClean)
                                  ? xPredAffineBlk(COMPONENT_Cb, pu, refPic, cMvTemp[refList][refIdxTemp], tmpBuf,
                                                   false, pu.cu->slice->clpRng(COMPONENT_Cb))
                                  : false;

          cMvTempValid[refList][refIdxTemp][0] = isSubPuYYClean && isSubPuCbClean;
          cMvTempValid[refList][refIdxTemp][1] = isSubPuYYClean && isSubPuCbClean;
          cMvTempValid[refList][refIdxTemp][2] = isSubPuYYClean && isSubPuCbClean;

          uiCostTempOk = true;
          uiCostTempOk =
            uiCostTempOk && cMvPredBiSolid[refList][refIdxTemp][0] && cMvPredBiSolid[refList][refIdxTemp][1];
          uiCostTempOk = uiCostTempOk && ((mvNum > 2) ? cMvPredBiSolid[refList][refIdxTemp][2] : true);
          uiCostTempOk = uiCostTempOk && cMvTempSolid[refList][refIdxTemp][0] && cMvTempSolid[refList][refIdxTemp][1]
                         && ((mvNum > 2) ? cMvTempSolid[refList][refIdxTemp][2] : true);
          uiCostTempOk = uiCostTempOk && cMvTempValid[refList][refIdxTemp][0] && cMvTempValid[refList][refIdxTemp][1]
                         && ((mvNum > 2) ? cMvTempValid[refList][refIdxTemp][2] : true);
        }
#endif

        xCopyAffineAMVPInfo(aacAffineAMVPInfo[refList][refIdxTemp], affiAMVPInfoTemp[eRefPicList]);
#if GDR_ENABLED
        if ( pu.cu->imv != 2 || !m_pcEncCfg->getUseAffineAmvrEncOpt() )
        {
          xCheckBestAffineMVP(pu, affiAMVPInfoTemp[eRefPicList], eRefPicList, cMvTemp[refList][refIdxTemp],
                              cMvPredBi[refList][refIdxTemp], aaiMvpIdxBi[refList][refIdxTemp], bitsTemp, costTemp);
          if (isEncodeGdrClean)
          {
            int mvpIdx = aaiMvpIdxBi[refList][refIdxTemp];

            cMvPredBiSolid[refList][refIdxTemp][0] = affiAMVPInfoTemp[eRefPicList].mvSolidLT[mvpIdx];
            cMvPredBiSolid[refList][refIdxTemp][1] = affiAMVPInfoTemp[eRefPicList].mvSolidRT[mvpIdx];
            cMvPredBiSolid[refList][refIdxTemp][2] = affiAMVPInfoTemp[eRefPicList].mvSolidLB[mvpIdx];

            cMvTempSolid[refList][refIdxTemp][0] = cMvPredBiSolid[refList][refIdxTemp][0];
            cMvTempSolid[refList][refIdxTemp][1] = cMvPredBiSolid[refList][refIdxTemp][1];
            cMvTempSolid[refList][refIdxTemp][2] = cMvPredBiSolid[refList][refIdxTemp][2];

            if (cMvTempValid[refList][refIdxTemp][0] && cMvTempValid[refList][refIdxTemp][1]
                && cMvTempValid[refList][refIdxTemp][2])
            {
              cMvTempValid[refList][refIdxTemp][0] = cMvPredBiSolid[refList][refIdxTemp][0];
              cMvTempValid[refList][refIdxTemp][1] = cMvPredBiSolid[refList][refIdxTemp][1];
              cMvTempValid[refList][refIdxTemp][2] = cMvPredBiSolid[refList][refIdxTemp][2];
            }

            uiCostTempOk = true;
            uiCostTempOk =
              uiCostTempOk && cMvPredBiSolid[refList][refIdxTemp][0] && cMvPredBiSolid[refList][refIdxTemp][1];
            uiCostTempOk = uiCostTempOk && ((mvNum > 2) ? cMvPredBiSolid[refList][refIdxTemp][2] : true);
            uiCostTempOk = uiCostTempOk && cMvTempSolid[refList][refIdxTemp][0] && cMvTempSolid[refList][refIdxTemp][1]
                           && ((mvNum > 2) ? cMvTempSolid[refList][refIdxTemp][2] : true);
            uiCostTempOk = uiCostTempOk && cMvTempValid[refList][refIdxTemp][0] && cMvTempValid[refList][refIdxTemp][1]
                           && ((mvNum > 2) ? cMvTempValid[refList][refIdxTemp][2] : true);
          }
        }
#else
        if ( pu.cu->imv != 2 || !m_pcEncCfg->getUseAffineAmvrEncOpt() )
        {
          xCheckBestAffineMVP(pu, affiAMVPInfoTemp[eRefPicList], eRefPicList, cMvTemp[refList][refIdxTemp],
                              cMvPredBi[refList][refIdxTemp], aaiMvpIdxBi[refList][refIdxTemp], bitsTemp, costTemp);
        }
#endif

#if GDR_ENABLED
        allOk = (costTemp < uiCostBi);

        if (isEncodeGdrClean)
        {
          if (uiCostTempOk)
          {
            allOk = (uiCostBiOk) ? (costTemp < uiCostBi) : true;
          }
          else
          {
            allOk = false;
          }
        }
#endif



#if GDR_ENABLED
        if (allOk)
#else
        if (costTemp < uiCostBi)
#endif
        {
          changed = true;
          ::memcpy(cMvBi[refList], cMvTemp[refList][refIdxTemp], sizeof(Mv) * 3);
#if GDR_ENABLED
          if (isEncodeGdrClean)
          {
            ::memcpy(cMvBiSolid[refList], cMvTempSolid[refList][refIdxTemp], sizeof(bool) * 3);
            ::memcpy(cMvBiValid[refList], cMvTempValid[refList][refIdxTemp], sizeof(bool) * 3);
          }
#endif
          iRefIdxBi[refList] = refIdxTemp;

          uiCostBi = costTemp;
#if GDR_ENABLED
          if (isEncodeGdrClean)
          {
            uiCostBiOk = uiCostTempOk;
          }
#endif
          motBits[refList] = bitsTemp - mbBits[2] - motBits[1 - refList];
          motBits[refList] -= ((pu.cu->slice->getSPS()->getUseBcw() == true) ? bcwIdxBits : 0);
          bits[2] = bitsTemp;

          if (numIter != 1)   // MC for next iter
          {
            //  Set motion
            PU::setAllAffineMv(pu, cMvBi[refList][0], cMvBi[refList][1], cMvBi[refList][2], eRefPicList);
            pu.refIdx[eRefPicList] = iRefIdxBi[eRefPicList];

#if GDR_ENABLED
            if (isEncodeGdrClean)
            {
              bool isSubPuYYClean;
              bool isSubPuCbClean;
              PelUnitBuf     tmpBuf = m_tmpAffiStorage.getBuf(UnitAreaRelative(*pu.cu, pu));
              const Picture *refPic = pu.cu->slice->getRefPic((RefPicList) refList, pu.refIdx[eRefPicList]);

              pu.mvAffiSolid[eRefPicList][0] = cMvBiSolid[refList][0];
              pu.mvAffiSolid[eRefPicList][1] = cMvBiSolid[refList][1];
              pu.mvAffiSolid[eRefPicList][2] = cMvBiSolid[refList][2];

              isSubPuYYClean = xPredAffineBlk(COMPONENT_Y, pu, refPic, cMvBi[refList], tmpBuf, false,
                                              pu.cu->slice->clpRng(COMPONENT_Y));
              isSubPuCbClean = (isSubPuYYClean) ? xPredAffineBlk(COMPONENT_Cb, pu, refPic, cMvBi[refList], tmpBuf,
                                                                 false, pu.cu->slice->clpRng(COMPONENT_Cb))
                                                : false;

              pu.mvAffiValid[eRefPicList][0] = cMvBiValid[refList][0] = isSubPuYYClean && isSubPuCbClean;
              pu.mvAffiValid[eRefPicList][1] = cMvBiValid[refList][1] = isSubPuYYClean && isSubPuCbClean;
              pu.mvAffiValid[eRefPicList][2] = cMvBiValid[refList][2] = isSubPuYYClean && isSubPuCbClean;
            }
#endif

            PelUnitBuf predBufTmp = m_tmpPredStorage[refList].getBuf(UnitAreaRelative(*pu.cu, pu));
            motionCompensation( pu, predBufTmp, eRefPicList );
          }
        }
      }   // for loop-refIdxTemp

      if (!changed)
      {
#if GDR_ENABLED
        allOk = ((uiCostBi <= uiCost[0] && uiCostBi <= uiCost[1]) || enforceBcwPred);

        if (isEncodeGdrClean)
        {
          if (uiCostBiOk)
          {
            allOk = (uiCostOk[0] && uiCostOk[1]) ? ((uiCostBi <= uiCost[0] && uiCostBi <= uiCost[1]) || enforceBcwPred) : true;
          }
          else
          {
            allOk = false;
          }
        }
#endif

#if GDR_ENABLED
        if (allOk)
#else
        if ((uiCostBi <= uiCost[0] && uiCostBi <= uiCost[1]) || enforceBcwPred)
#endif
        {
          xCopyAffineAMVPInfo( aacAffineAMVPInfo[0][iRefIdxBi[0]], affiAMVPInfoTemp[REF_PIC_LIST_0] );
          xCheckBestAffineMVP(pu, affiAMVPInfoTemp[REF_PIC_LIST_0], REF_PIC_LIST_0, cMvBi[0],
                              cMvPredBi[0][iRefIdxBi[0]], aaiMvpIdxBi[0][iRefIdxBi[0]], bits[2], uiCostBi);
#if GDR_ENABLED
          if (isEncodeGdrClean)
          {
            int mvpIdx = aaiMvpIdxBi[0][iRefIdxBi[0]];

            cMvPredBiSolid[REF_PIC_LIST_0][iRefIdxBi[0]][0] = affiAMVPInfoTemp[REF_PIC_LIST_0].mvSolidLT[mvpIdx];
            cMvPredBiSolid[REF_PIC_LIST_0][iRefIdxBi[0]][1] = affiAMVPInfoTemp[REF_PIC_LIST_0].mvSolidRT[mvpIdx];
            cMvPredBiSolid[REF_PIC_LIST_0][iRefIdxBi[0]][2] = affiAMVPInfoTemp[REF_PIC_LIST_0].mvSolidLB[mvpIdx];

            cMvBiSolid[REF_PIC_LIST_0][0] = cMvPredBiSolid[REF_PIC_LIST_0][iRefIdxBi[0]][0];
            cMvBiSolid[REF_PIC_LIST_0][1] = cMvPredBiSolid[REF_PIC_LIST_0][iRefIdxBi[0]][1];
            cMvBiSolid[REF_PIC_LIST_0][2] = cMvPredBiSolid[REF_PIC_LIST_0][iRefIdxBi[0]][2];

            if (cMvBiValid[REF_PIC_LIST_0][0] && cMvBiValid[REF_PIC_LIST_0][1] && cMvBiValid[REF_PIC_LIST_0][2])
            {
              cMvBiValid[REF_PIC_LIST_0][0] = cMvPredBiSolid[REF_PIC_LIST_0][iRefIdxBi[0]][0];
              cMvBiValid[REF_PIC_LIST_0][1] = cMvPredBiSolid[REF_PIC_LIST_0][iRefIdxBi[0]][1];
              cMvBiValid[REF_PIC_LIST_0][2] = cMvPredBiSolid[REF_PIC_LIST_0][iRefIdxBi[0]][2];
            }

            uiCostBiOk = true;
            uiCostBiOk = uiCostBiOk && cMvPredBiSolid[REF_PIC_LIST_0][iRefIdxBi[0]][0] && cMvPredBiSolid[REF_PIC_LIST_0][iRefIdxBi[0]][1];
            uiCostBiOk = uiCostBiOk && ((mvNum > 2) ? cMvPredBiSolid[REF_PIC_LIST_0][iRefIdxBi[0]][2] : true);
            uiCostBiOk = uiCostBiOk && cMvBiSolid[0][0] && cMvBiSolid[0][1] && ((mvNum > 2) ? cMvBiSolid[0][2] : true);
            uiCostBiOk = uiCostBiOk && cMvBiValid[0][0] && cMvBiValid[0][1] && ((mvNum > 2) ? cMvBiValid[0][2] : true);
          }
#endif

          if ( !slice.getPicHeader()->getMvdL1ZeroFlag() )
          {
            xCopyAffineAMVPInfo( aacAffineAMVPInfo[1][iRefIdxBi[1]], affiAMVPInfoTemp[REF_PIC_LIST_1] );
            xCheckBestAffineMVP(pu, affiAMVPInfoTemp[REF_PIC_LIST_1], REF_PIC_LIST_1, cMvBi[1],
                                cMvPredBi[1][iRefIdxBi[1]], aaiMvpIdxBi[1][iRefIdxBi[1]], bits[2], uiCostBi);
#if GDR_ENABLED
            if (isEncodeGdrClean)
            {
              int mvpIdx = aaiMvpIdxBi[1][iRefIdxBi[1]];

              cMvPredBiSolid[REF_PIC_LIST_1][iRefIdxBi[1]][0] = affiAMVPInfoTemp[REF_PIC_LIST_1].mvSolidLT[mvpIdx];
              cMvPredBiSolid[REF_PIC_LIST_1][iRefIdxBi[1]][1] = affiAMVPInfoTemp[REF_PIC_LIST_1].mvSolidRT[mvpIdx];
              cMvPredBiSolid[REF_PIC_LIST_1][iRefIdxBi[1]][2] = affiAMVPInfoTemp[REF_PIC_LIST_1].mvSolidLB[mvpIdx];

              cMvBiSolid[REF_PIC_LIST_1][0] = cMvPredBiSolid[REF_PIC_LIST_1][iRefIdxBi[1]][0];
              cMvBiSolid[REF_PIC_LIST_1][1] = cMvPredBiSolid[REF_PIC_LIST_1][iRefIdxBi[1]][1];
              cMvBiSolid[REF_PIC_LIST_1][2] = cMvPredBiSolid[REF_PIC_LIST_1][iRefIdxBi[1]][2];

              if (cMvBiValid[REF_PIC_LIST_1][0] && cMvBiValid[REF_PIC_LIST_1][1] && cMvBiValid[REF_PIC_LIST_1][2])
              {
                cMvBiValid[REF_PIC_LIST_1][0] = cMvPredBiSolid[REF_PIC_LIST_1][iRefIdxBi[1]][0];
                cMvBiValid[REF_PIC_LIST_1][1] = cMvPredBiSolid[REF_PIC_LIST_1][iRefIdxBi[1]][1];
                cMvBiValid[REF_PIC_LIST_1][2] = cMvPredBiSolid[REF_PIC_LIST_1][iRefIdxBi[1]][2];
              }

              uiCostBiOk = true;
              uiCostBiOk = uiCostBiOk && cMvPredBiSolid[REF_PIC_LIST_1][iRefIdxBi[1]][0] && cMvPredBiSolid[REF_PIC_LIST_1][iRefIdxBi[1]][1];
              uiCostBiOk = uiCostBiOk && ((mvNum > 2) ? cMvPredBiSolid[REF_PIC_LIST_1][iRefIdxBi[1]][2] : true);
              uiCostBiOk = uiCostBiOk && cMvBiSolid[1][0] && cMvBiSolid[1][1] && ((mvNum > 2) ? cMvBiSolid[1][2] : true);
              uiCostBiOk = uiCostBiOk && cMvBiValid[1][0] && cMvBiValid[1][1] && ((mvNum > 2) ? cMvBiValid[1][2] : true);
            }
#endif
          }
        }
        break;
      }
    } // for loop-iter
    }
    m_isBi = false;
  } // if (B_SLICE)

  pu.mv    [REF_PIC_LIST_0] = Mv();
  pu.mv    [REF_PIC_LIST_1] = Mv();
  pu.mvd   [REF_PIC_LIST_0] = cMvZero;
  pu.mvd   [REF_PIC_LIST_1] = cMvZero;
  pu.refIdx[REF_PIC_LIST_0] = NOT_VALID;
  pu.refIdx[REF_PIC_LIST_1] = NOT_VALID;
  pu.mvpIdx[REF_PIC_LIST_0] = NOT_VALID;
  pu.mvpIdx[REF_PIC_LIST_1] = NOT_VALID;
  pu.mvpNum[REF_PIC_LIST_0] = NOT_VALID;
  pu.mvpNum[REF_PIC_LIST_1] = NOT_VALID;

  for ( int verIdx = 0; verIdx < 3; verIdx++ )
  {
    pu.mvdAffi[REF_PIC_LIST_0][verIdx] = cMvZero;
    pu.mvdAffi[REF_PIC_LIST_1][verIdx] = cMvZero;
  }

  // Set Motion Field
  memcpy( aacMv[1], mvValidList1, sizeof(Mv)*3 );
  refIdx[1]  = refIdxValidList1;
  bits[1]    = bitsValidList1;
  uiCost[1]  = costValidList1;

#if GDR_ENABLED
  if (isEncodeGdrClean)
  {
    memcpy(aacMvSolid[1], mvValidList1Solid, sizeof(bool) * 3);
    memcpy(aacMvValid[1], mvValidList1Valid, sizeof(bool) * 3);
    uiCostOk[1] = costValidList1Ok;
  }
#endif
  if (pu.cs->pps->getWPBiPred() == true && tryBipred && (bcwIdx != BCW_DEFAULT))
  {
    CHECK(iRefIdxBi[0]<0, "Invalid picture reference index");
    CHECK(iRefIdxBi[1]<0, "Invalid picture reference index");
    wp0 = pu.cs->slice->getWpScaling(REF_PIC_LIST_0, iRefIdxBi[0]);
    wp1 = pu.cs->slice->getWpScaling(REF_PIC_LIST_1, iRefIdxBi[1]);

    if (WPScalingParam::isWeighted(wp0) || WPScalingParam::isWeighted(wp1))
    {
      uiCostBi = MAX_UINT;
      enforceBcwPred = false;
#if GDR_ENABLED
      uiCostBiOk = false;
#endif
    }
  }
  if( enforceBcwPred )
  {
    uiCost[0] = uiCost[1] = MAX_UINT;
#if GDR_ENABLED
    uiCostOk[0] = uiCostOk[1] = false;
#endif
  }

  // Affine ME result set
#if GDR_ENABLED
  bool BiOk = (uiCostBi <= uiCost[0] && uiCostBi <= uiCost[1]);

  if (isEncodeGdrClean)
  {
    if (uiCostBiOk)
      BiOk = (uiCostOk[0] && uiCostOk[1]) ? (uiCostBi <= uiCost[0] && uiCostBi <= uiCost[1]) : true;
    else
      BiOk = false;
  }

  bool L0ok = (uiCost[0] <= uiCost[1]);
  if (isEncodeGdrClean)
  {
    if (uiCostOk[0])
      L0ok = (uiCostOk[1]) ? (uiCost[0] <= uiCost[1]) : true;
    else
      L0ok = false;
  }
#endif

#if GDR_ENABLED
  if (BiOk)
#else
  if ( uiCostBi <= uiCost[0] && uiCostBi <= uiCost[1] ) // Bi
#endif
  {
    lastMode = 2;
    affineCost = uiCostBi;
    pu.interDir = 3;
    PU::setAllAffineMv( pu, cMvBi[0][0], cMvBi[0][1], cMvBi[0][2], REF_PIC_LIST_0);
    PU::setAllAffineMv( pu, cMvBi[1][0], cMvBi[1][1], cMvBi[1][2], REF_PIC_LIST_1);
    pu.refIdx[REF_PIC_LIST_0] = iRefIdxBi[0];
    pu.refIdx[REF_PIC_LIST_1] = iRefIdxBi[1];

#if GDR_ENABLED
    if (isEncodeGdrClean)
    {
      PelUnitBuf     tmpBuf = m_tmpAffiStorage.getBuf(UnitAreaRelative(*pu.cu, pu));
      const Picture *refPic0 = (pu.refIdx[REF_PIC_LIST_0] < 0) ? nullptr : pu.cu->slice->getRefPic((RefPicList)REF_PIC_LIST_0, pu.refIdx[REF_PIC_LIST_0]);
      const Picture *refPic1 = (pu.refIdx[REF_PIC_LIST_1] < 0) ? nullptr : pu.cu->slice->getRefPic((RefPicList)REF_PIC_LIST_1, pu.refIdx[REF_PIC_LIST_1]);

      pu.mvAffiSolid[REF_PIC_LIST_0][0] = cMvBiSolid[REF_PIC_LIST_0][0];
      pu.mvAffiSolid[REF_PIC_LIST_0][1] = cMvBiSolid[REF_PIC_LIST_0][1];
      pu.mvAffiSolid[REF_PIC_LIST_0][2] = cMvBiSolid[REF_PIC_LIST_0][2];

      bool isSubPuYYClean0 = false;
      bool isSubPuCbClean0 = false;
      if (refPic0) {
        isSubPuYYClean0 = xPredAffineBlk(COMPONENT_Y, pu, refPic0, cMvBi[0], tmpBuf, false, pu.cu->slice->clpRng(COMPONENT_Y));
        isSubPuCbClean0 = (isSubPuYYClean0) ? xPredAffineBlk(COMPONENT_Cb, pu, refPic0, cMvBi[0], tmpBuf, false, pu.cu->slice->clpRng(COMPONENT_Cb)) : false;
      }

      pu.mvAffiValid[REF_PIC_LIST_0][0] = cMvBiValid[REF_PIC_LIST_0][0] = isSubPuYYClean0 && isSubPuCbClean0;
      pu.mvAffiValid[REF_PIC_LIST_0][1] = cMvBiValid[REF_PIC_LIST_0][1] = isSubPuYYClean0 && isSubPuCbClean0;
      pu.mvAffiValid[REF_PIC_LIST_0][2] = cMvBiValid[REF_PIC_LIST_0][2] = isSubPuYYClean0 && isSubPuCbClean0;


      pu.mvAffiSolid[REF_PIC_LIST_1][0] = cMvBiSolid[REF_PIC_LIST_1][0];
      pu.mvAffiSolid[REF_PIC_LIST_1][1] = cMvBiSolid[REF_PIC_LIST_1][1];
      pu.mvAffiSolid[REF_PIC_LIST_1][2] = cMvBiSolid[REF_PIC_LIST_1][2];

      bool isSubPuYYClean1 = false;
      bool isSubPuCbClean1 = false;
      if (refPic1)
      {
        isSubPuYYClean1 = xPredAffineBlk(COMPONENT_Y, pu, refPic1, cMvBi[1], tmpBuf, false, pu.cu->slice->clpRng(COMPONENT_Y));
        isSubPuCbClean1 = (isSubPuYYClean1) ? xPredAffineBlk(COMPONENT_Cb, pu, refPic1, cMvBi[1], tmpBuf, false, pu.cu->slice->clpRng(COMPONENT_Cb)) : false;
      }

      pu.mvAffiValid[REF_PIC_LIST_1][0] = cMvBiValid[REF_PIC_LIST_1][0] = isSubPuYYClean1 && isSubPuCbClean1;
      pu.mvAffiValid[REF_PIC_LIST_1][1] = cMvBiValid[REF_PIC_LIST_1][1] = isSubPuYYClean1 && isSubPuCbClean1;
      pu.mvAffiValid[REF_PIC_LIST_1][2] = cMvBiValid[REF_PIC_LIST_1][2] = isSubPuYYClean1 && isSubPuCbClean1;
    }
#endif


    for ( int verIdx = 0; verIdx < mvNum; verIdx++ )
    {
      pu.mvdAffi[REF_PIC_LIST_0][verIdx] = cMvBi[0][verIdx] - cMvPredBi[0][iRefIdxBi[0]][verIdx];
      pu.mvdAffi[REF_PIC_LIST_1][verIdx] = cMvBi[1][verIdx] - cMvPredBi[1][iRefIdxBi[1]][verIdx];
      if ( verIdx != 0 )
      {
        pu.mvdAffi[0][verIdx] = pu.mvdAffi[0][verIdx] - pu.mvdAffi[0][0];
        pu.mvdAffi[1][verIdx] = pu.mvdAffi[1][verIdx] - pu.mvdAffi[1][0];
      }
    }

    pu.mvpIdx[REF_PIC_LIST_0] = aaiMvpIdxBi[0][iRefIdxBi[0]];
    pu.mvpNum[REF_PIC_LIST_0] = aaiMvpNum[0][iRefIdxBi[0]];
    pu.mvpIdx[REF_PIC_LIST_1] = aaiMvpIdxBi[1][iRefIdxBi[1]];
    pu.mvpNum[REF_PIC_LIST_1] = aaiMvpNum[1][iRefIdxBi[1]];

#if GDR_ENABLED
    if (isEncodeGdrClean)
    {
      pu.mvpSolid[REF_PIC_LIST_0] = affiAMVPInfoTemp[0].mvSolidLT[pu.mvpIdx[0]] && affiAMVPInfoTemp[0].mvSolidRT[pu.mvpIdx[0]];
      pu.mvpSolid[REF_PIC_LIST_1] = affiAMVPInfoTemp[1].mvSolidLT[pu.mvpIdx[1]] && affiAMVPInfoTemp[1].mvSolidRT[pu.mvpIdx[1]];

      if (pu.cu->affineType == AFFINEMODEL_6PARAM)
      {
        pu.mvpSolid[REF_PIC_LIST_0] = pu.mvpSolid[REF_PIC_LIST_0] && affiAMVPInfoTemp[0].mvSolidLB[pu.mvpIdx[0]];
        pu.mvpSolid[REF_PIC_LIST_1] = pu.mvpSolid[REF_PIC_LIST_1] && affiAMVPInfoTemp[1].mvSolidLB[pu.mvpIdx[1]];
      }
    }
#endif
  }
#if GDR_ENABLED
  else if (L0ok) // List 0
#else
  else if ( uiCost[0] <= uiCost[1] ) // List 0
#endif
  {
    lastMode = 0;
    affineCost = uiCost[0];
    pu.interDir = 1;
    PU::setAllAffineMv( pu, aacMv[0][0], aacMv[0][1], aacMv[0][2], REF_PIC_LIST_0);
    pu.refIdx[REF_PIC_LIST_0] = refIdx[0];

#if GDR_ENABLED
    if (isEncodeGdrClean)
    {
      bool isSubPuYYClean;
      bool isSubPuCbClean;
      PelUnitBuf     tmpBuf = m_tmpAffiStorage.getBuf(UnitAreaRelative(*pu.cu, pu));
      const Picture *refPic = pu.cu->slice->getRefPic((RefPicList)REF_PIC_LIST_0, pu.refIdx[REF_PIC_LIST_0]);

      pu.mvAffiSolid[0][0] = aacMvSolid[0][0];
      pu.mvAffiSolid[0][1] = aacMvSolid[0][1];
      pu.mvAffiSolid[0][2] = aacMvSolid[0][2];

      isSubPuYYClean = xPredAffineBlk(COMPONENT_Y, pu, refPic, aacMv[0], tmpBuf, false, pu.cu->slice->clpRng(COMPONENT_Y));
      isSubPuCbClean = (isSubPuYYClean) ? xPredAffineBlk(COMPONENT_Cb, pu, refPic, aacMv[0], tmpBuf, false, pu.cu->slice->clpRng(COMPONENT_Cb)) : false;

      pu.mvAffiValid[0][0] = aacMvValid[0][0] = isSubPuYYClean && isSubPuCbClean;
      pu.mvAffiValid[0][1] = aacMvValid[0][1] = isSubPuYYClean && isSubPuCbClean;
      pu.mvAffiValid[0][2] = aacMvValid[0][2] = isSubPuYYClean && isSubPuCbClean;
    }
#endif

    for ( int verIdx = 0; verIdx < mvNum; verIdx++ )
    {
      pu.mvdAffi[REF_PIC_LIST_0][verIdx] = aacMv[0][verIdx] - cMvPred[0][refIdx[0]][verIdx];
      if ( verIdx != 0 )
      {
        pu.mvdAffi[0][verIdx] = pu.mvdAffi[0][verIdx] - pu.mvdAffi[0][0];
      }
    }

    pu.mvpIdx[REF_PIC_LIST_0] = aaiMvpIdx[0][refIdx[0]];
    pu.mvpNum[REF_PIC_LIST_0] = aaiMvpNum[0][refIdx[0]];
#if GDR_ENABLED
    if (isEncodeGdrClean)
    {
      pu.mvpSolid[REF_PIC_LIST_0] = affiAMVPInfoTemp[0].mvSolidLT[pu.mvpIdx[0]] && affiAMVPInfoTemp[0].mvSolidRT[pu.mvpIdx[0]];

      if (pu.cu->affineType == AFFINEMODEL_6PARAM)
      {
        pu.mvpSolid[REF_PIC_LIST_0] = pu.mvpSolid[REF_PIC_LIST_0] && affiAMVPInfoTemp[0].mvSolidLB[pu.mvpIdx[0]];
      }
    }
#endif
  }
  else
  {
    lastMode = 1;
    affineCost = uiCost[1];
    pu.interDir = 2;
    PU::setAllAffineMv( pu, aacMv[1][0], aacMv[1][1], aacMv[1][2], REF_PIC_LIST_1);
    pu.refIdx[REF_PIC_LIST_1] = refIdx[1];

#if GDR_ENABLED
    if (isEncodeGdrClean)
    {
      bool isSubPuYYClean;
      bool isSubPuCbClean;
      PelUnitBuf     tmpBuf = m_tmpAffiStorage.getBuf(UnitAreaRelative(*pu.cu, pu));
      const Picture *refPic = pu.cu->slice->getRefPic((RefPicList)REF_PIC_LIST_1, pu.refIdx[REF_PIC_LIST_1]);

      pu.mvAffiSolid[1][0] = aacMvSolid[1][0];
      pu.mvAffiSolid[1][1] = aacMvSolid[1][1];
      pu.mvAffiSolid[1][2] = aacMvSolid[1][2];

      isSubPuYYClean = xPredAffineBlk(COMPONENT_Y, pu, refPic, aacMv[1], tmpBuf, false, pu.cu->slice->clpRng(COMPONENT_Y));
      isSubPuCbClean = (isSubPuYYClean) ? xPredAffineBlk(COMPONENT_Cb, pu, refPic, aacMv[1], tmpBuf, false, pu.cu->slice->clpRng(COMPONENT_Cb)) : false;

      pu.mvAffiValid[1][0] = aacMvValid[1][0] = isSubPuYYClean && isSubPuCbClean;
      pu.mvAffiValid[1][1] = aacMvValid[1][1] = isSubPuYYClean && isSubPuCbClean;
      pu.mvAffiValid[1][2] = aacMvValid[1][2] = isSubPuYYClean && isSubPuCbClean;
    }
#endif

    for ( int verIdx = 0; verIdx < mvNum; verIdx++ )
    {
      pu.mvdAffi[REF_PIC_LIST_1][verIdx] = aacMv[1][verIdx] - cMvPred[1][refIdx[1]][verIdx];
      if ( verIdx != 0 )
      {
        pu.mvdAffi[1][verIdx] = pu.mvdAffi[1][verIdx] - pu.mvdAffi[1][0];
      }
    }

    pu.mvpIdx[REF_PIC_LIST_1] = aaiMvpIdx[1][refIdx[1]];
    pu.mvpNum[REF_PIC_LIST_1] = aaiMvpNum[1][refIdx[1]];
#if GDR_ENABLED
    if (isEncodeGdrClean)
    {
      pu.mvpSolid[REF_PIC_LIST_1] = affiAMVPInfoTemp[1].mvSolidLT[pu.mvpIdx[1]] && affiAMVPInfoTemp[1].mvSolidRT[pu.mvpIdx[1]];

      if (pu.cu->affineType == AFFINEMODEL_6PARAM)
      {
        pu.mvpSolid[REF_PIC_LIST_1] = pu.mvpSolid[REF_PIC_LIST_1] && affiAMVPInfoTemp[1].mvSolidLB[pu.mvpIdx[1]];
      }
    }
#endif
  }
  if( bcwIdx != BCW_DEFAULT )
  {
    pu.cu->BcwIdx = BCW_DEFAULT;
  }

//RAMIRO: função para calcular e extrair features
#if EXTRA_FEATURES
    if(pu.cu->affineType == AFFINEMODEL_4PARAM){    //VTM 16.2
    //if (pu.cu->affineType == AffineModel::_4_PARAMS){     //VTM 22.0

        affCustoMvUniL0 = uiCost[0];
        affCustoMvUniL1 = uiCost[1];
        affCustoMvBi = uiCostBi;    //VTM 16.2
        //affCustoMvBi = costBi;  //VTM 22.0

        affMvUniL0P0HorX = aacMv[0][0].getHor();
        affMvUniL0P0VerY = aacMv[0][0].getVer();
        affMvUniL0P1HorX = aacMv[0][1].getHor();
        affMvUniL0P1VerY = aacMv[0][1].getVer();

        affMvUniL1P0HorX = aacMv[1][0].getHor();
        affMvUniL1P0VerY = aacMv[1][0].getVer();
        affMvUniL1P1HorX = aacMv[1][1].getHor();
        affMvUniL1P1VerY = aacMv[1][1].getVer();

        affMvPredUniL0P0HorX = cMvPred[0][refIdx[0]][0].getHor();
        affMvPredUniL0P0VerY = cMvPred[0][refIdx[0]][0].getVer();
        affMvPredUniL0P1HorX = cMvPred[0][refIdx[0]][1].getHor();
        affMvPredUniL0P1VerY = cMvPred[0][refIdx[0]][1].getVer();

        affMvPredUniL1P0HorX = cMvPred[1][refIdx[1]][0].getHor();
        affMvPredUniL1P0VerY = cMvPred[1][refIdx[1]][0].getVer();
        affMvPredUniL1P1HorX = cMvPred[1][refIdx[1]][1].getHor();
        affMvPredUniL1P1VerY = cMvPred[1][refIdx[1]][1].getVer();

        affMvBiL0P0HorX = cMvBi[0][0].getHor();
        affMvBiL0P0VerY = cMvBi[0][0].getVer();
        affMvBiL0P1HorX = cMvBi[0][1].getHor();
        affMvBiL0P1VerY = cMvBi[0][1].getVer();

        affMvBiL1P0HorX = cMvBi[1][0].getHor();
        affMvBiL1P0VerY = cMvBi[1][0].getVer();
        affMvBiL1P1HorX = cMvBi[1][1].getHor();
        affMvBiL1P1VerY = cMvBi[1][1].getVer(); 

        affMvPredBiL0P0HorX = cMvPredBi[0][iRefIdxBi[0]][0].getHor();
        affMvPredBiL0P0VerY = cMvPredBi[0][iRefIdxBi[0]][0].getVer();
        affMvPredBiL0P1HorX = cMvPredBi[0][iRefIdxBi[0]][1].getHor();
        affMvPredBiL0P1VerY = cMvPredBi[0][iRefIdxBi[0]][1].getVer();

        affMvPredBiL1P0HorX = cMvPredBi[1][iRefIdxBi[1]][0].getHor();
        affMvPredBiL1P0VerY = cMvPredBi[1][iRefIdxBi[1]][0].getVer();
        affMvPredBiL1P1HorX = cMvPredBi[1][iRefIdxBi[1]][1].getHor();
        affMvPredBiL1P1VerY = cMvPredBi[1][iRefIdxBi[1]][1].getVer();  

        affBitsMvBi = bits[2];

        affInterDir = (int) pu.interDir;
    }
#endif

}

void solveEqual(double dEqualCoeff[7][7], int order, double *dAffinePara)
{
  for (int k = 0; k < order; k++)
  {
    dAffinePara[k] = 0.;
  }

  // row echelon
  for (int i = 1; i < order; i++)
  {
    // find column max
    double temp = fabs(dEqualCoeff[i][i-1]);
    int tempIdx = i;
    for (int j = i + 1; j < order + 1; j++)
    {
      if ( fabs(dEqualCoeff[j][i-1]) > temp )
      {
        temp = fabs(dEqualCoeff[j][i-1]);
        tempIdx = j;
      }
    }

    // swap line
    if ( tempIdx != i )
    {
      for (int j = 0; j < order + 1; j++)
      {
        dEqualCoeff[0][j] = dEqualCoeff[i][j];
        dEqualCoeff[i][j] = dEqualCoeff[tempIdx][j];
        dEqualCoeff[tempIdx][j] = dEqualCoeff[0][j];
      }
    }

    // elimination first column
    if ( dEqualCoeff[i][i - 1] == 0. )
    {
      return;
    }
    for (int j = i + 1; j < order + 1; j++)
    {
      for (int k = i; k < order + 1; k++)
      {
        dEqualCoeff[j][k] = dEqualCoeff[j][k] - dEqualCoeff[i][k] * dEqualCoeff[j][i-1] / dEqualCoeff[i][i-1];
      }
    }
  }

  if (dEqualCoeff[order][order - 1] == 0.)
  {
    return;
  }
  dAffinePara[order - 1] = dEqualCoeff[order][order] / dEqualCoeff[order][order - 1];
  for (int i = order - 2; i >= 0; i--)
  {
    if ( dEqualCoeff[i + 1][i] == 0. )
    {
      for (int k = 0; k < order; k++)
      {
        dAffinePara[k] = 0.;
      }
      return;
    }
    double temp = 0;
    for (int j = i + 1; j < order; j++)
    {
      temp += dEqualCoeff[i+1][j] * dAffinePara[j];
    }
    dAffinePara[i] = (dEqualCoeff[i + 1][order] - temp) / dEqualCoeff[i + 1][i];
  }
}

void InterSearch::xCheckBestAffineMVP( PredictionUnit &pu, AffineAMVPInfo &affineAMVPInfo, RefPicList eRefPicList, Mv acMv[3], Mv acMvPred[3], int& riMVPIdx, uint32_t& ruiBits, Distortion& ruiCost )
{
#if GDR_ENABLED
  const CodingStructure &cs = *pu.cs;
  const bool isEncodeGdrClean = cs.sps->getGDREnabledFlag() && cs.pcv->isEncoder && ((cs.picHeader->getInGdrInterval() && cs.isClean(pu.Y().topRight(), CHANNEL_TYPE_LUMA)) || (cs.picHeader->getNumVerVirtualBoundaries() == 0));
#endif

  if ( affineAMVPInfo.numCand < 2 )
  {
    return;
  }

  int mvNum = pu.cu->affineType ? 3 : 2;

  m_pcRdCost->selectMotionLambda( );
  m_pcRdCost->setCostScale ( 0 );

  int iBestMVPIdx = riMVPIdx;

  // Get origin MV bits
  Mv tmpPredMv[3];
  int iOrgMvBits = xCalcAffineMVBits( pu, acMv, acMvPred );
  iOrgMvBits += m_auiMVPIdxCost[riMVPIdx][AMVP_MAX_NUM_CANDS];

  int iBestMvBits = iOrgMvBits;
  for (int iMVPIdx = 0; iMVPIdx < affineAMVPInfo.numCand; iMVPIdx++)
  {
    if (iMVPIdx == riMVPIdx)
    {
      continue;
    }
    tmpPredMv[0] = affineAMVPInfo.mvCandLT[iMVPIdx];
    tmpPredMv[1] = affineAMVPInfo.mvCandRT[iMVPIdx];
    if ( mvNum == 3 )
    {
      tmpPredMv[2] = affineAMVPInfo.mvCandLB[iMVPIdx];
    }
    int iMvBits = xCalcAffineMVBits( pu, acMv, tmpPredMv );
    iMvBits += m_auiMVPIdxCost[iMVPIdx][AMVP_MAX_NUM_CANDS];

#if GDR_ENABLED
    bool allOk = (iMvBits < iBestMvBits);
    if (isEncodeGdrClean)
    {
      bool curOk = affineAMVPInfo.mvSolidLT[iMVPIdx] && affineAMVPInfo.mvSolidRT[iMVPIdx];
      if (pu.cu->affineType == AFFINEMODEL_6PARAM)
      {
        curOk = curOk && affineAMVPInfo.mvSolidLB[iMVPIdx];
      }

      bool best_ok = affineAMVPInfo.mvSolidLT[iBestMVPIdx] && affineAMVPInfo.mvSolidRT[iBestMVPIdx];
      if (pu.cu->affineType == AFFINEMODEL_6PARAM)
      {
        curOk = curOk && affineAMVPInfo.mvSolidLB[iBestMVPIdx];
      }

      if (curOk)
      {
        allOk = (best_ok) ? (iMvBits < iBestMvBits) : true;
      }
      else
      {
        allOk = false;
      }
    }
#endif


#if GDR_ENABLED
    if (allOk)
#else
    if (iMvBits < iBestMvBits)
#endif
    {
      iBestMvBits = iMvBits;
      iBestMVPIdx = iMVPIdx;
    }
  }

  if (iBestMVPIdx != riMVPIdx)  // if changed
  {
    acMvPred[0] = affineAMVPInfo.mvCandLT[iBestMVPIdx];
    acMvPred[1] = affineAMVPInfo.mvCandRT[iBestMVPIdx];
    acMvPred[2] = affineAMVPInfo.mvCandLB[iBestMVPIdx];
    riMVPIdx = iBestMVPIdx;
    uint32_t uiOrgBits = ruiBits;
    ruiBits = uiOrgBits - iOrgMvBits + iBestMvBits;
    ruiCost = (ruiCost - m_pcRdCost->getCost( uiOrgBits )) + m_pcRdCost->getCost( ruiBits );
  }
}

//RAMIRO: chama a funcao AffineMotionEstimation
#if GDR_ENABLED
void InterSearch::xAffineMotionEstimation(PredictionUnit &pu, PelUnitBuf &origBuf, RefPicList eRefPicList,
                                          Mv acMvPred[3], int refIdxPred, Mv acMv[3], bool acMvSolid[3],
                                          uint32_t &ruiBits, Distortion &ruiCost, int &mvpIdx,
                                          const AffineAMVPInfo &aamvpi, bool &rbCleanCandExist, bool bBi)
#else
void InterSearch::xAffineMotionEstimation(PredictionUnit &pu, PelUnitBuf &origBuf, RefPicList eRefPicList,
                                          Mv acMvPred[3], int refIdxPred, Mv acMv[3], uint32_t &ruiBits,
                                          Distortion &ruiCost, int &mvpIdx, const AffineAMVPInfo &aamvpi, bool bBi)
#endif
{
#if GDR_ENABLED
  if (pu.cu->cs->sps->getUseBcw() && pu.cu->BcwIdx != BCW_DEFAULT && !bBi
      && xReadBufferedAffineUniMv(pu, eRefPicList, refIdxPred, acMvPred, acMv, acMvSolid, ruiBits, ruiCost, mvpIdx,
                                  aamvpi))
#else
  if (pu.cu->cs->sps->getUseBcw() && pu.cu->BcwIdx != BCW_DEFAULT && !bBi
      && xReadBufferedAffineUniMv(pu, eRefPicList, refIdxPred, acMvPred, acMv, ruiBits, ruiCost, mvpIdx, aamvpi))
#endif
  {
    return;
  }
#if GDR_ENABLED
  const CodingStructure &cs = *pu.cs;
  const bool isEncodeGdrClean = cs.sps->getGDREnabledFlag() && cs.pcv->isEncoder && ((cs.picHeader->getInGdrInterval() && cs.isClean(pu.Y().topRight(), CHANNEL_TYPE_LUMA)) || (cs.picHeader->getNumVerVirtualBoundaries() == 0));
  bool acMvValid[3];
#endif

  uint32_t dirBits = ruiBits - m_auiMVPIdxCost[mvpIdx][aamvpi.numCand];
  int bestMvpIdx   = mvpIdx;
  const int width  = pu.Y().width;
  const int height = pu.Y().height;

  const Picture *refPic = pu.cu->slice->getRefPic(eRefPicList, refIdxPred);

  // Set Origin YUV: pcYuv
  PelUnitBuf*   pBuf = &origBuf;
  double        fWeight       = 1.0;

  PelUnitBuf  origBufTmp = m_tmpStorageLCU.getBuf( UnitAreaRelative( *pu.cu, pu ) );
  enum DFunc distFunc = (pu.cs->slice->getDisableSATDForRD()) ? DF_SAD : DF_HAD;
  m_iRefListIdx = eRefPicList;

  // if Bi, set to ( 2 * Org - ListX )
  if ( bBi )
  {
    // NOTE: Other buf contains predicted signal from another direction
    PelUnitBuf otherBuf = m_tmpPredStorage[1 - (int)eRefPicList].getBuf( UnitAreaRelative( *pu.cu, pu ) );
    origBufTmp.copyFrom(origBuf);
    origBufTmp.removeHighFreq(otherBuf, m_pcEncCfg->getClipForBiPredMeEnabled(), pu.cu->slice->clpRngs()
                             ,getBcwWeight(pu.cu->BcwIdx, eRefPicList)
                             );
    pBuf = &origBufTmp;

    fWeight = xGetMEDistortionWeight( pu.cu->BcwIdx, eRefPicList );
  }

  // pred YUV
  PelUnitBuf  predBuf = m_tmpAffiStorage.getBuf( UnitAreaRelative(*pu.cu, pu) );

  // Set start Mv position, use input mv as started search mv
  Mv acMvTemp[3];
  ::memcpy( acMvTemp, acMv, sizeof(Mv)*3 );

#if GDR_ENABLED
  bool acMvTempSolid[3];
  ::memcpy(acMvTempSolid, acMvSolid, sizeof(bool) * 3);
#endif
  // Set delta mv
  // malloc buffer
  int iParaNum = pu.cu->affineType ? 7 : 5;
  int affineParaNum = iParaNum - 1;
  int mvNum = pu.cu->affineType ? 3 : 2;
  double pdEqualCoeff[7][7];

  int64_t  i64EqualCoeff[7][7];
  Pel    *piError = m_tmpAffiError;
  int    *pdDerivate[2];
  pdDerivate[0] = m_tmpAffiDeri[0];
  pdDerivate[1] = m_tmpAffiDeri[1];

  Distortion uiCostBest = std::numeric_limits<Distortion>::max();
  uint32_t uiBitsBest = 0;
#if GDR_ENABLED
  bool uiCostBestOk = true;
  bool uiCostTempOk = true;
  bool costTempOk = true;

  bool allOk = true;
#endif

  // do motion compensation with origin mv
  if( m_pcEncCfg->getMCTSEncConstraint() )
  {
    Area curTileAreaRestricted = pu.cs->picture->mctsInfo.getTileAreaSubPelRestricted( pu );
    MCTSHelper::clipMvToArea( acMvTemp[0], pu.cu->Y(), curTileAreaRestricted, *pu.cs->sps );
    MCTSHelper::clipMvToArea( acMvTemp[1], pu.cu->Y(), curTileAreaRestricted, *pu.cs->sps );
    if( pu.cu->affineType == AFFINEMODEL_6PARAM )
    {
      MCTSHelper::clipMvToArea( acMvTemp[2], pu.cu->Y(), curTileAreaRestricted, *pu.cs->sps );
    }
  }
  else
  {
    clipMv( acMvTemp[0], pu.cu->lumaPos(), pu.cu->lumaSize(), *pu.cs->sps, *pu.cs->pps );
    clipMv( acMvTemp[1], pu.cu->lumaPos(), pu.cu->lumaSize(), *pu.cs->sps, *pu.cs->pps );
    if( pu.cu->affineType == AFFINEMODEL_6PARAM )
    {
      clipMv( acMvTemp[2], pu.cu->lumaPos(), pu.cu->lumaSize(), *pu.cs->sps, *pu.cs->pps );
    }
  }
  acMvTemp[0].roundAffinePrecInternal2Amvr(pu.cu->imv);
  acMvTemp[1].roundAffinePrecInternal2Amvr(pu.cu->imv);
  if (pu.cu->affineType == AFFINEMODEL_6PARAM)
  {
    acMvTemp[2].roundAffinePrecInternal2Amvr(pu.cu->imv);
  }
#if GDR_ENABLED
  bool YYOk = xPredAffineBlk(COMPONENT_Y, pu, refPic, acMvTemp, predBuf, false, pu.cs->slice->clpRng(COMPONENT_Y));
#else
  xPredAffineBlk( COMPONENT_Y, pu, refPic, acMvTemp, predBuf, false, pu.cs->slice->clpRng( COMPONENT_Y ) );
#endif

  // get error
  uiCostBest = m_pcRdCost->getDistPart(predBuf.Y(), pBuf->Y(), pu.cs->sps->getBitDepth(CHANNEL_TYPE_LUMA), COMPONENT_Y, distFunc);

  // get cost with mv
  m_pcRdCost->setCostScale(0);
  uiBitsBest = ruiBits;
  if ( pu.cu->imv == 2 && m_pcEncCfg->getUseAffineAmvrEncOpt() )
  {
    uiBitsBest  = dirBits + xDetermineBestMvp( pu, acMvTemp, mvpIdx, aamvpi );
    acMvPred[0] = aamvpi.mvCandLT[mvpIdx];
    acMvPred[1] = aamvpi.mvCandRT[mvpIdx];
    acMvPred[2] = aamvpi.mvCandLB[mvpIdx];
  }
  else
  {
    DTRACE( g_trace_ctx, D_COMMON, " (%d) xx uiBitsBest=%d\n", DTRACE_GET_COUNTER(g_trace_ctx,D_COMMON), uiBitsBest );
    uiBitsBest += xCalcAffineMVBits( pu, acMvTemp, acMvPred );
    DTRACE( g_trace_ctx, D_COMMON, " (%d) yy uiBitsBest=%d\n", DTRACE_GET_COUNTER(g_trace_ctx,D_COMMON), uiBitsBest );
  }

#if GDR_ENABLED
  if (isEncodeGdrClean)
  {
    acMvSolid[0] = aamvpi.mvSolidLT[mvpIdx];
    acMvSolid[1] = aamvpi.mvSolidRT[mvpIdx];
    acMvSolid[2] = aamvpi.mvSolidLB[mvpIdx];

    bool isSubPuYYClean = YYOk;
    bool isSubPuCbClean = true;

    acMvValid[0] = isSubPuYYClean && isSubPuCbClean;
    acMvValid[1] = isSubPuYYClean && isSubPuCbClean;
    acMvValid[2] = isSubPuYYClean && isSubPuCbClean;

    uiCostBestOk = (acMvSolid[0] && acMvSolid[1] && acMvSolid[2]) && (acMvValid[0] && acMvValid[1] && acMvValid[2]);
  }
#endif

  uiCostBest = (Distortion)( floor( fWeight * (double)uiCostBest ) + (double)m_pcRdCost->getCost( uiBitsBest ) );

  DTRACE( g_trace_ctx, D_COMMON, " (%d) uiBitsBest=%d, uiCostBest=%d\n", DTRACE_GET_COUNTER(g_trace_ctx,D_COMMON), uiBitsBest, uiCostBest );

  ::memcpy( acMv, acMvTemp, sizeof(Mv) * 3 );

  const int bufStride = pBuf->Y().stride;
  const int predBufStride = predBuf.Y().stride;
  Mv prevIterMv[7][3];
  int iIterTime;
  if ( pu.cu->affineType == AFFINEMODEL_6PARAM )
  {
    iIterTime = bBi ? 3 : 4;
  }
  else
  {
    iIterTime = bBi ? 3 : 5;
  }

  if ( !pu.cu->cs->sps->getUseAffineType() )
  {
    iIterTime = bBi ? 5 : 7;
  }
  for ( int iter=0; iter<iIterTime; iter++ )    // iterate loop
  {
    memcpy( prevIterMv[iter], acMvTemp, sizeof( Mv ) * 3 );
    /*********************************************************************************
     *                         use gradient to update mv
     *********************************************************************************/
    // get Error Matrix
    Pel* pOrg  = pBuf->Y().buf;
    Pel* pPred = predBuf.Y().buf;
    for ( int j=0; j< height; j++ )
    {
      for ( int i=0; i< width; i++ )
      {
        piError[i + j * width] = pOrg[i] - pPred[i];
      }
      pOrg  += bufStride;
      pPred += predBufStride;
    }

    // sobel x direction
    // -1 0 1
    // -2 0 2
    // -1 0 1
    pPred = predBuf.Y().buf;
    m_HorizontalSobelFilter( pPred, predBufStride, pdDerivate[0], width, width, height );

    // sobel y direction
    // -1 -2 -1
    //  0  0  0
    //  1  2  1
    m_VerticalSobelFilter( pPred, predBufStride, pdDerivate[1], width, width, height );

    // solve delta x and y
    for ( int row = 0; row < iParaNum; row++ )
    {
      memset( &i64EqualCoeff[row][0], 0, iParaNum * sizeof( int64_t ) );
    }

    m_EqualCoeffComputer( piError, width, pdDerivate, width, i64EqualCoeff, width, height
      , (pu.cu->affineType == AFFINEMODEL_6PARAM)
    );

    for ( int row = 0; row < iParaNum; row++ )
    {
      for ( int i = 0; i < iParaNum; i++ )
      {
        pdEqualCoeff[row][i] = (double)i64EqualCoeff[row][i];
      }
    }

    double dAffinePara[6];
    double dDeltaMv[6]={0.0, 0.0, 0.0, 0.0, 0.0, 0.0,};
    Mv acDeltaMv[3];

    solveEqual( pdEqualCoeff, affineParaNum, dAffinePara );

    // convert to delta mv
    dDeltaMv[0] = dAffinePara[0];
    dDeltaMv[2] = dAffinePara[2];
    if ( pu.cu->affineType == AFFINEMODEL_6PARAM )
    {
      dDeltaMv[1] = dAffinePara[1] * width + dAffinePara[0];
      dDeltaMv[3] = dAffinePara[3] * width + dAffinePara[2];
      dDeltaMv[4] = dAffinePara[4] * height + dAffinePara[0];
      dDeltaMv[5] = dAffinePara[5] * height + dAffinePara[2];
    }
    else
    {
      dDeltaMv[1] = dAffinePara[1] * width + dAffinePara[0];
      dDeltaMv[3] = -dAffinePara[3] * width + dAffinePara[2];
    }

    for (int i = 0; i < 6; i++)
    {
      dDeltaMv[i] = Clip3(-8192.0, 8192.0, dDeltaMv[i]);
    }

    const int normShiftTab[3] = { MV_PRECISION_QUARTER - MV_PRECISION_INT, MV_PRECISION_SIXTEENTH - MV_PRECISION_INT, MV_PRECISION_QUARTER - MV_PRECISION_INT };
    const int stepShiftTab[3] = { MV_PRECISION_INTERNAL - MV_PRECISION_QUARTER, MV_PRECISION_INTERNAL - MV_PRECISION_SIXTEENTH, MV_PRECISION_INTERNAL - MV_PRECISION_QUARTER };
    const int multiShift = 1 << normShiftTab[pu.cu->imv];
    const int mvShift = stepShiftTab[pu.cu->imv];

    acDeltaMv[0] = Mv((int) (dDeltaMv[0] * multiShift + SIGN(dDeltaMv[0]) * 0.5) * (1 << mvShift),
                      (int) (dDeltaMv[2] * multiShift + SIGN(dDeltaMv[2]) * 0.5) * (1 << mvShift));
    acDeltaMv[1] = Mv((int) (dDeltaMv[1] * multiShift + SIGN(dDeltaMv[1]) * 0.5) * (1 << mvShift),
                      (int) (dDeltaMv[3] * multiShift + SIGN(dDeltaMv[3]) * 0.5) * (1 << mvShift));

    if ( pu.cu->affineType == AFFINEMODEL_6PARAM )
    {
      acDeltaMv[2] = Mv((int) (dDeltaMv[4] * multiShift + SIGN(dDeltaMv[4]) * 0.5) * (1 << mvShift),
                        (int) (dDeltaMv[5] * multiShift + SIGN(dDeltaMv[5]) * 0.5) * (1 << mvShift));
    }
    if ( !m_pcEncCfg->getUseAffineAmvrEncOpt() )
    {
      bool bAllZero = false;
      for ( int i = 0; i < mvNum; i++ )
      {
        Mv deltaMv = acDeltaMv[i];
        if ( pu.cu->imv == 2 )
        {
          deltaMv.roundToPrecision( MV_PRECISION_INTERNAL, MV_PRECISION_HALF );
        }
        if ( deltaMv.getHor() != 0 || deltaMv.getVer() != 0 )
        {
          bAllZero = false;
          break;
        }
        bAllZero = true;
      }

      if ( bAllZero )
        break;
    }
    // do motion compensation with updated mv
    for ( int i = 0; i < mvNum; i++ )
    {
      acMvTemp[i] += acDeltaMv[i];
      acMvTemp[i].hor = Clip3(MV_MIN, MV_MAX, acMvTemp[i].hor );
      acMvTemp[i].ver = Clip3(MV_MIN, MV_MAX, acMvTemp[i].ver );
      acMvTemp[i].roundAffinePrecInternal2Amvr(pu.cu->imv);
      if( m_pcEncCfg->getMCTSEncConstraint() )
      {
        MCTSHelper::clipMvToArea( acMvTemp[i], pu.cu->Y(), pu.cs->picture->mctsInfo.getTileAreaSubPelRestricted( pu ), *pu.cs->sps );
      }
      else
      {
        clipMv( acMvTemp[i], pu.cu->lumaPos(), pu.cu->lumaSize(), *pu.cs->sps, *pu.cs->pps );
      }
    }

    if ( m_pcEncCfg->getUseAffineAmvrEncOpt() )
    {
      bool identical = false;
      for ( int k = iter; k >= 0; k-- )
      {
        if ( acMvTemp[0] == prevIterMv[k][0] && acMvTemp[1] == prevIterMv[k][1] )
        {
          identical = pu.cu->affineType ? acMvTemp[2] == prevIterMv[k][2] : true;
          if ( identical )
          {
            break;
          }
        }
      }
      if ( identical )
      {
        break;
      }
    }

#if GDR_ENABLED
    bool YYOk = xPredAffineBlk(COMPONENT_Y, pu, refPic, acMvTemp, predBuf, false, pu.cu->slice->clpRng(COMPONENT_Y));
#else
    xPredAffineBlk( COMPONENT_Y, pu, refPic, acMvTemp, predBuf, false, pu.cu->slice->clpRng( COMPONENT_Y ) );
#endif

    // get error
    Distortion costTemp = m_pcRdCost->getDistPart(predBuf.Y(), pBuf->Y(), pu.cs->sps->getBitDepth(CHANNEL_TYPE_LUMA),
                                                  COMPONENT_Y, distFunc);
    DTRACE(g_trace_ctx, D_COMMON, " (%d) costTemp=%d\n", DTRACE_GET_COUNTER(g_trace_ctx, D_COMMON), costTemp);

    // get cost with mv
    m_pcRdCost->setCostScale(0);
    uint32_t bitsTemp = ruiBits;
    if ( pu.cu->imv == 2 && m_pcEncCfg->getUseAffineAmvrEncOpt() )
    {
      bitsTemp    = dirBits + xDetermineBestMvp(pu, acMvTemp, bestMvpIdx, aamvpi);
      acMvPred[0] = aamvpi.mvCandLT[bestMvpIdx];
      acMvPred[1] = aamvpi.mvCandRT[bestMvpIdx];
      acMvPred[2] = aamvpi.mvCandLB[bestMvpIdx];
    }
    else
    {
      bitsTemp += xCalcAffineMVBits(pu, acMvTemp, acMvPred);
    }
#if GDR_ENABLED
    if (isEncodeGdrClean)
    {
      acMvSolid[0] = aamvpi.mvSolidLT[bestMvpIdx];
      acMvSolid[1] = aamvpi.mvSolidRT[bestMvpIdx];
      acMvSolid[2] = aamvpi.mvSolidLB[bestMvpIdx];

      bool isSubPuYYClean = YYOk;
      bool isSubPuCbClean = true;

      acMvValid[0] = isSubPuYYClean && isSubPuCbClean;
      acMvValid[1] = isSubPuYYClean && isSubPuCbClean;
      acMvValid[2] = isSubPuYYClean && isSubPuCbClean;

      uiCostTempOk = (acMvSolid[0] && acMvSolid[1] && acMvSolid[2]) && (acMvValid[0] && acMvValid[1] && acMvValid[2]);
    }
#endif

    costTemp = (Distortion) (floor(fWeight * (double) costTemp) + (double) m_pcRdCost->getCost(bitsTemp));

    // store best cost and mv
#if GDR_ENABLED
    allOk = (costTemp < uiCostBest);
    if (isEncodeGdrClean)
    {
      if (uiCostTempOk)
      {
        allOk = (uiCostBestOk) ? (costTemp < uiCostBest) : true;
      }
      else
      {
        allOk = false;
      }
    }

    if (allOk)
#else
    if (costTemp < uiCostBest)
#endif
    {
      uiCostBest = costTemp;
#if GDR_ENABLED
      if (isEncodeGdrClean)
      {
        uiCostBestOk = uiCostTempOk;
      }
#endif
      uiBitsBest = bitsTemp;
      memcpy( acMv, acMvTemp, sizeof(Mv) * 3 );
      mvpIdx = bestMvpIdx;
    }
  }

  auto checkCPMVRdCost = [&](Mv ctrlPtMv[3])
  {
#if GDR_ENABLED
    bool YYOk = xPredAffineBlk(COMPONENT_Y, pu, refPic, ctrlPtMv, predBuf, false, pu.cu->slice->clpRng(COMPONENT_Y));
#else
    xPredAffineBlk(COMPONENT_Y, pu, refPic, ctrlPtMv, predBuf, false, pu.cu->slice->clpRng(COMPONENT_Y));
#endif

#if GDR_ENABLED
    if (isEncodeGdrClean)
    {
      acMvSolid[0] = aamvpi.mvSolidLT[bestMvpIdx];
      acMvSolid[1] = aamvpi.mvSolidRT[bestMvpIdx];
      acMvSolid[2] = aamvpi.mvSolidLB[bestMvpIdx];

      bool isSubPuYYClean = YYOk;
      bool isSubPuCbClean = true; // (isSubPuYYClean) ? xPredAffineBlk(COMPONENT_Cb, pu, refPic, ctrlPtMv, tmpBuf, false, pu.cu->slice->clpRng(COMPONENT_Cb)) : false;

      acMvValid[0] = isSubPuYYClean && isSubPuCbClean;
      acMvValid[1] = isSubPuYYClean && isSubPuCbClean;
      acMvValid[2] = isSubPuYYClean && isSubPuCbClean;

      costTempOk = (acMvSolid[0] && acMvSolid[1] && acMvSolid[2]) && (acMvValid[0] && acMvValid[1] && acMvValid[2]);
    }
#endif

    // get error
    Distortion costTemp = m_pcRdCost->getDistPart(predBuf.Y(), pBuf->Y(), pu.cs->sps->getBitDepth(CHANNEL_TYPE_LUMA), COMPONENT_Y, distFunc);
    // get cost with mv
    m_pcRdCost->setCostScale(0);
    uint32_t bitsTemp = ruiBits;
    bitsTemp += xCalcAffineMVBits( pu, ctrlPtMv, acMvPred );
    costTemp = (Distortion)(floor(fWeight * (double)costTemp) + (double)m_pcRdCost->getCost(bitsTemp));
    // store best cost and mv
#if GDR_ENABLED
    bool allOk = (costTemp < uiCostBest);
    if (isEncodeGdrClean)
    {
      if (costTempOk)
      {
        allOk = (uiCostBestOk) ? (costTemp < uiCostBest) : true;
      }
      else
      {
        allOk = false;
      }
    }

    if (allOk)
#else
    if (costTemp < uiCostBest)
#endif
    {
      uiCostBest = costTemp;
#if GDR_ENABLED
      if (isEncodeGdrClean)
      {
        uiCostBestOk = costTempOk;
        rbCleanCandExist = true;
      }
#endif
      uiBitsBest = bitsTemp;
      ::memcpy(acMv, ctrlPtMv, sizeof(Mv) * 3);
    }
  };

  const uint32_t mvShiftTable[3] = {MV_PRECISION_INTERNAL - MV_PRECISION_QUARTER, MV_PRECISION_INTERNAL - MV_PRECISION_INTERNAL, MV_PRECISION_INTERNAL - MV_PRECISION_INT};
  const uint32_t mvShift = mvShiftTable[pu.cu->imv];
  if (uiCostBest <= AFFINE_ME_LIST_MVP_TH*m_hevcCost)
  {

    Mv mvPredTmp[3] = { acMvPred[0], acMvPred[1], acMvPred[2] };
    Mv mvME[3];
    ::memcpy(mvME, acMv, sizeof(Mv) * 3);
    Mv dMv = mvME[0] - mvPredTmp[0];

    for (int j = 0; j < mvNum; j++)
    {
      if ((!j && mvME[j] != mvPredTmp[j]) || (j && mvME[j] != (mvPredTmp[j] + dMv)))
      {
        ::memcpy(acMvTemp, mvME, sizeof(Mv) * 3);
        acMvTemp[j] = mvPredTmp[j];

        if (j)
        {
          acMvTemp[j] += dMv;
        }

        checkCPMVRdCost(acMvTemp);
      }
    }

    //keep the rotation/zoom;
    if (mvME[0] != mvPredTmp[0])
    {
      ::memcpy(acMvTemp, mvME, sizeof(Mv) * 3);
      for (int i = 1; i < mvNum; i++)
      {
        acMvTemp[i] -= dMv;
      }
      acMvTemp[0] = mvPredTmp[0];

      checkCPMVRdCost(acMvTemp);
    }

    //keep the translation;
    if (pu.cu->affineType == AFFINEMODEL_6PARAM && mvME[1] != (mvPredTmp[1] + dMv) && mvME[2] != (mvPredTmp[2] + dMv))
    {
      ::memcpy(acMvTemp, mvME, sizeof(Mv) * 3);

      acMvTemp[1] = mvPredTmp[1] + dMv;
      acMvTemp[2] = mvPredTmp[2] + dMv;

      checkCPMVRdCost(acMvTemp);
    }

    // 8 nearest neighbor search
    int testPos[8][2] = { { -1, 0 },{ 0, -1 },{ 0, 1 },{ 1, 0 },{ -1, -1 },{ -1, 1 },{ 1, 1 },{ 1, -1 } };
    const int maxSearchRound = (pu.cu->imv) ? 3 : ((m_pcEncCfg->getUseAffineAmvrEncOpt() && m_pcEncCfg->getIsLowDelay()) ? 2 : 3);

    for (int rnd = 0; rnd < maxSearchRound; rnd++)
    {
      bool modelChange = false;
      //search the model parameters with finear granularity;
      for (int j = 0; j < mvNum; j++)
      {
        bool loopChange = false;
        for (int iter = 0; iter < 2; iter++)
        {
          if (iter == 1 && !loopChange)
          {
            break;
          }
          Mv centerMv[3];
          memcpy(centerMv, acMv, sizeof(Mv) * 3);
          memcpy(acMvTemp, acMv, sizeof(Mv) * 3);

          for (int i = ((iter == 0) ? 0 : 4); i < ((iter == 0) ? 4 : 8); i++)
          {
            acMvTemp[j].set(centerMv[j].getHor() + testPos[i][0] * (1 << mvShift),
                            centerMv[j].getVer() + testPos[i][1] * (1 << mvShift));
            clipMv( acMvTemp[j], pu.cu->lumaPos(), pu.cu->lumaSize(), *pu.cs->sps, *pu.cs->pps );
#if GDR_ENABLED
            bool YYOk = xPredAffineBlk(COMPONENT_Y, pu, refPic, acMvTemp, predBuf, false, pu.cu->slice->clpRng(COMPONENT_Y));
#else
            xPredAffineBlk(COMPONENT_Y, pu, refPic, acMvTemp, predBuf, false, pu.cu->slice->clpRng(COMPONENT_Y));
#endif

#if GDR_ENABLED
            if (isEncodeGdrClean)
            {
              acMvSolid[0] = aamvpi.mvSolidLT[bestMvpIdx];
              acMvSolid[1] = aamvpi.mvSolidRT[bestMvpIdx];
              acMvSolid[2] = aamvpi.mvSolidLB[bestMvpIdx];

              bool isSubPuYYClean = YYOk;
              bool isSubPuCbClean = true;

              acMvValid[0] = isSubPuYYClean && isSubPuCbClean;
              acMvValid[1] = isSubPuYYClean && isSubPuCbClean;
              acMvValid[2] = isSubPuYYClean && isSubPuCbClean;

              costTempOk = (acMvSolid[0] && acMvSolid[1] && acMvSolid[2]) && (acMvValid[0] && acMvValid[1] && acMvValid[2]);
            }
#endif

            Distortion costTemp = m_pcRdCost->getDistPart(predBuf.Y(), pBuf->Y(), pu.cs->sps->getBitDepth(CHANNEL_TYPE_LUMA), COMPONENT_Y, distFunc);
            uint32_t bitsTemp = ruiBits;
            bitsTemp += xCalcAffineMVBits(pu, acMvTemp, acMvPred);
            costTemp = (Distortion)(floor(fWeight * (double)costTemp) + (double)m_pcRdCost->getCost(bitsTemp));

#if GDR_ENABLED
            bool allOk = (costTemp < uiCostBest);
            if (isEncodeGdrClean)
            {
              if (costTempOk)
              {
                allOk = (uiCostBestOk) ? (costTemp < uiCostBest) : true;
              }
              else
              {
                allOk = false;
              }
            }

            if (allOk)
#else
            if (costTemp < uiCostBest)
#endif
            {
              uiCostBest = costTemp;
#if GDR_ENABLED
              if (isEncodeGdrClean)
              {
                uiCostBestOk = costTempOk;
                rbCleanCandExist = true;
              }
#endif
              uiBitsBest = bitsTemp;
              ::memcpy(acMv, acMvTemp, sizeof(Mv) * 3);
              modelChange = true;
              loopChange = true;
            }
          }
        }
      }

      if (!modelChange)
      {
        break;
      }
    }
  }
  acMvPred[0] = aamvpi.mvCandLT[mvpIdx];
  acMvPred[1] = aamvpi.mvCandRT[mvpIdx];
  acMvPred[2] = aamvpi.mvCandLB[mvpIdx];

#if GDR_ENABLED
  acMvSolid[0] = aamvpi.mvSolidLT[mvpIdx];
  acMvSolid[1] = aamvpi.mvSolidRT[mvpIdx];
  acMvSolid[2] = aamvpi.mvSolidLB[mvpIdx];
#endif

  ruiBits = uiBitsBest;
  ruiCost = uiCostBest;
  DTRACE( g_trace_ctx, D_COMMON, " (%d) uiBitsBest=%d, uiCostBest=%d\n", DTRACE_GET_COUNTER(g_trace_ctx,D_COMMON), uiBitsBest, uiCostBest );
}

void InterSearch::xEstimateAffineAMVP(PredictionUnit &pu, AffineAMVPInfo &affineAMVPInfo, PelUnitBuf &origBuf,
                                      RefPicList eRefPicList, int refIdx, Mv acMvPred[3], Distortion *puiDistBiP)
{
  Mv         bestMvLT, bestMvRT, bestMvLB;
  int        iBestIdx = 0;
  Distortion uiBestCost = std::numeric_limits<Distortion>::max();

#if GDR_ENABLED
  const CodingStructure &cs = *pu.cs;
  const bool isEncodeGdrClean = cs.sps->getGDREnabledFlag() && cs.pcv->isEncoder && ((cs.picHeader->getInGdrInterval() && cs.isClean(pu.Y().topRight(), CHANNEL_TYPE_LUMA)) || (cs.picHeader->getNumVerVirtualBoundaries() == 0));
  bool uiBestCostOk = false;
#endif

  // Fill the MV Candidates
  PU::fillAffineMvpCand(pu, eRefPicList, refIdx, affineAMVPInfo);
  CHECK( affineAMVPInfo.numCand == 0, "Assertion failed." );

  PelUnitBuf predBuf = m_tmpStorageLCU.getBuf( UnitAreaRelative(*pu.cu, pu) );

  // initialize Mvp index & Mvp
  iBestIdx = 0;
  for( int i = 0 ; i < affineAMVPInfo.numCand; i++ )
  {
    Mv mv[3] = { affineAMVPInfo.mvCandLT[i], affineAMVPInfo.mvCandRT[i], affineAMVPInfo.mvCandLB[i] };

#if GDR_ENABLED
    bool uiTmpCostOk = true;
    Distortion uiTmpCost =
      xGetAffineTemplateCost(pu, origBuf, predBuf, mv, i, AMVP_MAX_NUM_CANDS, eRefPicList, refIdx, uiTmpCostOk);
    uiTmpCostOk = uiTmpCostOk && affineAMVPInfo.mvSolidLT[i] && affineAMVPInfo.mvSolidRT[i];
    uiTmpCostOk = uiTmpCostOk && ((pu.cu->affineType == AFFINEMODEL_6PARAM) ? affineAMVPInfo.mvSolidLB[i] : true);
#else
    Distortion uiTmpCost = xGetAffineTemplateCost(pu, origBuf, predBuf, mv, i, AMVP_MAX_NUM_CANDS, eRefPicList, refIdx);
#endif

#if GDR_ENABLED
    bool allOk = uiBestCost > uiTmpCost;

    if (isEncodeGdrClean)
    {
      if (uiTmpCostOk)
      {
        allOk = uiBestCostOk ? (uiBestCost > uiTmpCost) : true;
      }
      else
      {
        allOk = false;
      }
    }
#endif

#if GDR_ENABLED
    if (allOk)
#else
    if ( uiBestCost > uiTmpCost )
#endif
    {
      uiBestCost = uiTmpCost;
      bestMvLT = affineAMVPInfo.mvCandLT[i];
      bestMvRT = affineAMVPInfo.mvCandRT[i];
      bestMvLB = affineAMVPInfo.mvCandLB[i];
      iBestIdx  = i;
      *puiDistBiP = uiTmpCost;
#if GDR_ENABLED
      if (isEncodeGdrClean)
      {
        uiBestCostOk = uiTmpCostOk;
      }
#endif
    }
  }

  // Setting Best MVP
  acMvPred[0] = bestMvLT;
  acMvPred[1] = bestMvRT;
  acMvPred[2] = bestMvLB;

  pu.mvpIdx[eRefPicList] = iBestIdx;
  pu.mvpNum[eRefPicList] = affineAMVPInfo.numCand;

#if GDR_ENABLED
  pu.mvpSolid[eRefPicList] = uiBestCostOk;
#endif
  DTRACE( g_trace_ctx, D_COMMON, "#estAffi=%d \n", affineAMVPInfo.numCand );
}

void InterSearch::xCopyAffineAMVPInfo (AffineAMVPInfo& src, AffineAMVPInfo& dst)
{
  dst.numCand = src.numCand;
  DTRACE( g_trace_ctx, D_COMMON, " (%d) #copyAffi=%d \n", DTRACE_GET_COUNTER( g_trace_ctx, D_COMMON ), src.numCand );
  ::memcpy( dst.mvCandLT, src.mvCandLT, sizeof(Mv)*src.numCand );
  ::memcpy( dst.mvCandRT, src.mvCandRT, sizeof(Mv)*src.numCand );
  ::memcpy( dst.mvCandLB, src.mvCandLB, sizeof(Mv)*src.numCand );

#if GDR_ENABLED
  ::memcpy(dst.mvSolidLT, src.mvSolidLT, sizeof(bool)*src.numCand);
  ::memcpy(dst.mvSolidRT, src.mvSolidRT, sizeof(bool)*src.numCand);
  ::memcpy(dst.mvSolidLB, src.mvSolidLB, sizeof(bool)*src.numCand);

  ::memcpy(dst.mvValidLT, src.mvValidLT, sizeof(bool)*src.numCand);
  ::memcpy(dst.mvValidRT, src.mvValidRT, sizeof(bool)*src.numCand);
  ::memcpy(dst.mvValidLB, src.mvValidLB, sizeof(bool)*src.numCand);

  ::memcpy(dst.mvTypeLT, src.mvTypeLT, sizeof(MvpType)*src.numCand);
  ::memcpy(dst.mvTypeRT, src.mvTypeRT, sizeof(MvpType)*src.numCand);
  ::memcpy(dst.mvTypeLB, src.mvTypeLB, sizeof(MvpType)*src.numCand);

  ::memcpy(dst.mvPosLT, src.mvPosLT, sizeof(Position)*src.numCand);
  ::memcpy(dst.mvPosRT, src.mvPosRT, sizeof(Position)*src.numCand);
  ::memcpy(dst.mvPosLB, src.mvPosLB, sizeof(Position)*src.numCand);
#endif
}


/**
* \brief Generate half-sample interpolated block
*
* \param pattern Reference picture ROI
* \param biPred    Flag indicating whether block is for biprediction
*/
void InterSearch::xExtDIFUpSamplingH(CPelBuf* pattern, bool useAltHpelIf)
{
  const ClpRng& clpRng = m_lumaClpRng;
  int width      = pattern->width;
  int height     = pattern->height;
  int srcStride  = pattern->stride;

  int intStride = width + 1;
  int dstStride = width + 1;
  Pel *intPtr;
  Pel *dstPtr;
  int filterSize = NTAPS_LUMA;
  int halfFilterSize = (filterSize>>1);
  const Pel *srcPtr = pattern->buf - halfFilterSize*srcStride - 1;

  m_if.filterHor(COMPONENT_Y, srcPtr, srcStride, m_filteredBlockTmp[0][0], intStride, width + 1, height + filterSize,
                 0 << MV_FRACTIONAL_BITS_DIFF, false, clpRng, 0, false, useAltHpelIf);
  if (!m_skipFracME)
  {
    m_if.filterHor(COMPONENT_Y, srcPtr, srcStride, m_filteredBlockTmp[2][0], intStride, width + 1, height + filterSize,
                   2 << MV_FRACTIONAL_BITS_DIFF, false, clpRng, 0, false, useAltHpelIf);
  }

  intPtr = m_filteredBlockTmp[0][0] + halfFilterSize * intStride + 1;
  dstPtr = m_filteredBlock[0][0][0];
  m_if.filterVer(COMPONENT_Y, intPtr, intStride, dstPtr, dstStride, width + 0, height + 0, 0 << MV_FRACTIONAL_BITS_DIFF,
                 false, true, clpRng, 0, false, useAltHpelIf);
  if (m_skipFracME)
  {
    return;
  }

  intPtr = m_filteredBlockTmp[0][0] + (halfFilterSize - 1) * intStride + 1;
  dstPtr = m_filteredBlock[2][0][0];
  m_if.filterVer(COMPONENT_Y, intPtr, intStride, dstPtr, dstStride, width + 0, height + 1, 2 << MV_FRACTIONAL_BITS_DIFF,
                 false, true, clpRng, 0, false, useAltHpelIf);

  intPtr = m_filteredBlockTmp[2][0] + halfFilterSize * intStride;
  dstPtr = m_filteredBlock[0][2][0];
  m_if.filterVer(COMPONENT_Y, intPtr, intStride, dstPtr, dstStride, width + 1, height + 0, 0 << MV_FRACTIONAL_BITS_DIFF,
                 false, true, clpRng, 0, false, useAltHpelIf);

  intPtr = m_filteredBlockTmp[2][0] + (halfFilterSize - 1) * intStride;
  dstPtr = m_filteredBlock[2][2][0];
  m_if.filterVer(COMPONENT_Y, intPtr, intStride, dstPtr, dstStride, width + 1, height + 1, 2 << MV_FRACTIONAL_BITS_DIFF,
                 false, true, clpRng, 0, false, useAltHpelIf);
}


/**
* \brief Generate quarter-sample interpolated blocks
*
* \param pattern    Reference picture ROI
* \param halfPelRef Half-pel mv
* \param biPred     Flag indicating whether block is for biprediction
*/
void InterSearch::xExtDIFUpSamplingQ( CPelBuf* pattern, Mv halfPelRef )
{
  const ClpRng& clpRng = m_lumaClpRng;
  int width      = pattern->width;
  int height     = pattern->height;
  int srcStride  = pattern->stride;

  Pel const* srcPtr;
  int intStride = width + 1;
  int dstStride = width + 1;
  Pel *intPtr;
  Pel *dstPtr;
  int filterSize = NTAPS_LUMA;

  int halfFilterSize = (filterSize>>1);

  int extHeight = (halfPelRef.getVer() == 0) ? height + filterSize : height + filterSize-1;

  // Horizontal filter 1/4
  srcPtr = pattern->buf - halfFilterSize * srcStride - 1;
  intPtr = m_filteredBlockTmp[1][0];
  if (halfPelRef.getVer() > 0)
  {
    srcPtr += srcStride;
  }
  if (halfPelRef.getHor() >= 0)
  {
    srcPtr += 1;
  }
  m_if.filterHor(COMPONENT_Y, srcPtr, srcStride, intPtr, intStride, width, extHeight, 1 << MV_FRACTIONAL_BITS_DIFF,
                 false, clpRng);

  // Horizontal filter 3/4
  srcPtr = pattern->buf - halfFilterSize*srcStride - 1;
  intPtr = m_filteredBlockTmp[3][0];
  if (halfPelRef.getVer() > 0)
  {
    srcPtr += srcStride;
  }
  if (halfPelRef.getHor() > 0)
  {
    srcPtr += 1;
  }
  m_if.filterHor(COMPONENT_Y, srcPtr, srcStride, intPtr, intStride, width, extHeight, 3 << MV_FRACTIONAL_BITS_DIFF,
                 false, clpRng);

  // Generate @ 1,1
  intPtr = m_filteredBlockTmp[1][0] + (halfFilterSize-1) * intStride;
  dstPtr = m_filteredBlock[1][1][0];
  if (halfPelRef.getVer() == 0)
  {
    intPtr += intStride;
  }
  m_if.filterVer(COMPONENT_Y, intPtr, intStride, dstPtr, dstStride, width, height, 1 << MV_FRACTIONAL_BITS_DIFF, false,
                 true, clpRng);

  // Generate @ 3,1
  intPtr = m_filteredBlockTmp[1][0] + (halfFilterSize-1) * intStride;
  dstPtr = m_filteredBlock[3][1][0];
  m_if.filterVer(COMPONENT_Y, intPtr, intStride, dstPtr, dstStride, width, height, 3 << MV_FRACTIONAL_BITS_DIFF, false,
                 true, clpRng);

  if (halfPelRef.getVer() != 0)
  {
    // Generate @ 2,1
    intPtr = m_filteredBlockTmp[1][0] + (halfFilterSize - 1) * intStride;
    dstPtr = m_filteredBlock[2][1][0];
    if (halfPelRef.getVer() == 0)
    {
      intPtr += intStride;
    }
    m_if.filterVer(COMPONENT_Y, intPtr, intStride, dstPtr, dstStride, width, height, 2 << MV_FRACTIONAL_BITS_DIFF,
                   false, true, clpRng);

    // Generate @ 2,3
    intPtr = m_filteredBlockTmp[3][0] + (halfFilterSize - 1) * intStride;
    dstPtr = m_filteredBlock[2][3][0];
    if (halfPelRef.getVer() == 0)
    {
      intPtr += intStride;
    }
    m_if.filterVer(COMPONENT_Y, intPtr, intStride, dstPtr, dstStride, width, height, 2 << MV_FRACTIONAL_BITS_DIFF,
                   false, true, clpRng);
  }
  else
  {
    // Generate @ 0,1
    intPtr = m_filteredBlockTmp[1][0] + halfFilterSize * intStride;
    dstPtr = m_filteredBlock[0][1][0];
    m_if.filterVer(COMPONENT_Y, intPtr, intStride, dstPtr, dstStride, width, height, 0 << MV_FRACTIONAL_BITS_DIFF,
                   false, true, clpRng);

    // Generate @ 0,3
    intPtr = m_filteredBlockTmp[3][0] + halfFilterSize * intStride;
    dstPtr = m_filteredBlock[0][3][0];
    m_if.filterVer(COMPONENT_Y, intPtr, intStride, dstPtr, dstStride, width, height, 0 << MV_FRACTIONAL_BITS_DIFF,
                   false, true, clpRng);
  }

  if (halfPelRef.getHor() != 0)
  {
    // Generate @ 1,2
    intPtr = m_filteredBlockTmp[2][0] + (halfFilterSize - 1) * intStride;
    dstPtr = m_filteredBlock[1][2][0];
    if (halfPelRef.getHor() > 0)
    {
      intPtr += 1;
    }
    if (halfPelRef.getVer() >= 0)
    {
      intPtr += intStride;
    }
    m_if.filterVer(COMPONENT_Y, intPtr, intStride, dstPtr, dstStride, width, height, 1 << MV_FRACTIONAL_BITS_DIFF,
                   false, true, clpRng);

    // Generate @ 3,2
    intPtr = m_filteredBlockTmp[2][0] + (halfFilterSize - 1) * intStride;
    dstPtr = m_filteredBlock[3][2][0];
    if (halfPelRef.getHor() > 0)
    {
      intPtr += 1;
    }
    if (halfPelRef.getVer() > 0)
    {
      intPtr += intStride;
    }
    m_if.filterVer(COMPONENT_Y, intPtr, intStride, dstPtr, dstStride, width, height, 3 << MV_FRACTIONAL_BITS_DIFF,
                   false, true, clpRng);
  }
  else
  {
    // Generate @ 1,0
    intPtr = m_filteredBlockTmp[0][0] + (halfFilterSize - 1) * intStride + 1;
    dstPtr = m_filteredBlock[1][0][0];
    if (halfPelRef.getVer() >= 0)
    {
      intPtr += intStride;
    }
    m_if.filterVer(COMPONENT_Y, intPtr, intStride, dstPtr, dstStride, width, height, 1 << MV_FRACTIONAL_BITS_DIFF,
                   false, true, clpRng);

    // Generate @ 3,0
    intPtr = m_filteredBlockTmp[0][0] + (halfFilterSize - 1) * intStride + 1;
    dstPtr = m_filteredBlock[3][0][0];
    if (halfPelRef.getVer() > 0)
    {
      intPtr += intStride;
    }
    m_if.filterVer(COMPONENT_Y, intPtr, intStride, dstPtr, dstStride, width, height, 3 << MV_FRACTIONAL_BITS_DIFF,
                   false, true, clpRng);
  }

  // Generate @ 1,3
  intPtr = m_filteredBlockTmp[3][0] + (halfFilterSize - 1) * intStride;
  dstPtr = m_filteredBlock[1][3][0];
  if (halfPelRef.getVer() == 0)
  {
    intPtr += intStride;
  }
  m_if.filterVer(COMPONENT_Y, intPtr, intStride, dstPtr, dstStride, width, height, 1 << MV_FRACTIONAL_BITS_DIFF, false,
                 true, clpRng);

  // Generate @ 3,3
  intPtr = m_filteredBlockTmp[3][0] + (halfFilterSize - 1) * intStride;
  dstPtr = m_filteredBlock[3][3][0];
  m_if.filterVer(COMPONENT_Y, intPtr, intStride, dstPtr, dstStride, width, height, 3 << MV_FRACTIONAL_BITS_DIFF, false,
                 true, clpRng);
}





//! set wp tables
void InterSearch::setWpScalingDistParam(int refIdx, RefPicList eRefPicListCur, Slice *pcSlice)
{
  if (refIdx < 0)
  {
    m_cDistParam.applyWeight = false;
    return;
  }

  WPScalingParam  *wp0 , *wp1;

  m_cDistParam.applyWeight = ( pcSlice->getSliceType()==P_SLICE && pcSlice->testWeightPred() ) || ( pcSlice->getSliceType()==B_SLICE && pcSlice->testWeightBiPred() ) ;

  if ( !m_cDistParam.applyWeight )
  {
    return;
  }

  int refIdx0 = (eRefPicListCur == REF_PIC_LIST_0) ? refIdx : (-1);
  int refIdx1 = (eRefPicListCur == REF_PIC_LIST_1) ? refIdx : (-1);

  getWpScaling(pcSlice, refIdx0, refIdx1, wp0, wp1);

  if (refIdx0 < 0)
  {
    wp0 = nullptr;
  }
  if (refIdx1 < 0)
  {
    wp1 = nullptr;
  }

  m_cDistParam.wpCur = nullptr;

  if ( eRefPicListCur == REF_PIC_LIST_0 )
  {
    m_cDistParam.wpCur = wp0;
  }
  else
  {
    m_cDistParam.wpCur = wp1;
  }
}

void InterSearch::xEncodeInterResidualQT(CodingStructure &cs, Partitioner &partitioner, const ComponentID &compID)
{
  const UnitArea& currArea    = partitioner.currArea();
  const TransformUnit &currTU = *cs.getTU(isLuma(partitioner.chType) ? currArea.lumaPos() : currArea.chromaPos(), partitioner.chType);
  const CodingUnit &cu        = *currTU.cu;
  const unsigned currDepth    = partitioner.currTrDepth;

  const bool bSubdiv          = currDepth != currTU.depth;

  if (compID == MAX_NUM_TBLOCKS)  // we are not processing a channel, instead we always recurse and code the CBFs
  {
    if( partitioner.canSplit( TU_MAX_TR_SPLIT, cs ) )
    {
      CHECK( !bSubdiv, "Not performing the implicit TU split" );
    }
    else if( cu.sbtInfo && partitioner.canSplit( PartSplit( cu.getSbtTuSplit() ), cs ) )
    {
      CHECK( !bSubdiv, "Not performing the implicit TU split - sbt" );
    }
    else
    {
      CHECK( bSubdiv, "transformsplit not supported" );
    }

    CHECK(CU::isIntra(cu), "Inter search provided with intra CU");

    if( cu.chromaFormat != CHROMA_400
      && (!cu.isSepTree() || isChroma(partitioner.chType))
      )
    {
      {
        const bool chroma_cbf = TU::getCbfAtDepth(currTU, COMPONENT_Cb, currDepth);
        if (!(cu.sbtInfo && (currDepth == 0 || (currDepth == 1 && currTU.noResidual))))
          m_CABACEstimator->cbf_comp(cs, chroma_cbf, currArea.blocks[COMPONENT_Cb], currDepth);
      }
      {
        const bool chroma_cbf = TU::getCbfAtDepth(currTU, COMPONENT_Cr, currDepth);
        if (!(cu.sbtInfo && (currDepth == 0 || (currDepth == 1 && currTU.noResidual))))
          m_CABACEstimator->cbf_comp(cs, chroma_cbf, currArea.blocks[COMPONENT_Cr], currDepth,
                                     TU::getCbfAtDepth(currTU, COMPONENT_Cb, currDepth));
      }
    }

    if( !bSubdiv && !( cu.sbtInfo && currTU.noResidual )
      && !isChroma(partitioner.chType)
      )
    {
      m_CABACEstimator->cbf_comp( cs, TU::getCbfAtDepth( currTU, COMPONENT_Y, currDepth ), currArea.Y(), currDepth );
    }
  }

  if (!bSubdiv)
  {
    if (compID != MAX_NUM_TBLOCKS) // we have already coded the CBFs, so now we code coefficients
    {
      if( currArea.blocks[compID].valid() )
      {
        if( compID == COMPONENT_Cr )
        {
          const int cbfMask = ( TU::getCbf( currTU, COMPONENT_Cb ) ? 2 : 0) + ( TU::getCbf( currTU, COMPONENT_Cr ) ? 1 : 0 );
          m_CABACEstimator->joint_cb_cr( currTU, cbfMask );
        }
        if( TU::getCbf( currTU, compID ) )
        {
          m_CABACEstimator->residual_coding( currTU, compID );
        }
      }
    }
  }
  else
  {
    if( compID == MAX_NUM_TBLOCKS || TU::getCbfAtDepth( currTU, compID, currDepth ) )
    {
      if( partitioner.canSplit( TU_MAX_TR_SPLIT, cs ) )
      {
        partitioner.splitCurrArea( TU_MAX_TR_SPLIT, cs );
      }
      else if( cu.sbtInfo && partitioner.canSplit( PartSplit( cu.getSbtTuSplit() ), cs ) )
      {
        partitioner.splitCurrArea( PartSplit( cu.getSbtTuSplit() ), cs );
      }
      else
      {
        THROW( "Implicit TU split not available!" );
      }

      do
      {
        xEncodeInterResidualQT( cs, partitioner, compID );
      } while( partitioner.nextPart( cs ) );

      partitioner.exitCurrSplit();
    }
  }
}

void InterSearch::calcMinDistSbt( CodingStructure &cs, const CodingUnit& cu, const uint8_t sbtAllowed )
{
  if( !sbtAllowed )
  {
    m_estMinDistSbt[NUMBER_SBT_MODE] = 0;
    for( int comp = 0; comp < getNumberValidTBlocks( *cs.pcv ); comp++ )
    {
      const ComponentID compID = ComponentID( comp );
      CPelBuf pred = cs.getPredBuf( compID );
      CPelBuf org  = cs.getOrgBuf( compID );
      m_estMinDistSbt[NUMBER_SBT_MODE] += m_pcRdCost->getDistPart( org, pred, cs.sps->getBitDepth( toChannelType( compID ) ), compID, DF_SSE );
    }
    return;
  }

  //SBT fast algorithm 2.1 : estimate a minimum RD cost of a SBT mode based on the luma distortion of uncoded part and coded part (assuming distorted can be reduced to 1/16);
  //                         if this cost is larger than the best cost, no need to try a specific SBT mode
  int cuWidth  = cu.lwidth();
  int cuHeight = cu.lheight();
  int numPartX = cuWidth  >= 16 ? 4 : ( cuWidth  == 4 ? 1 : 2 );
  int numPartY = cuHeight >= 16 ? 4 : ( cuHeight == 4 ? 1 : 2 );
  Distortion dist[4][4];
  memset( dist, 0, sizeof( Distortion ) * 16 );

  for( uint32_t c = 0; c < getNumberValidTBlocks( *cs.pcv ); c++ )
  {
    const ComponentID compID   = ComponentID( c );
    const CompArea&   compArea = cu.blocks[compID];
    const CPelBuf orgPel  = cs.getOrgBuf( compArea );
    const CPelBuf predPel = cs.getPredBuf( compArea );
    int lengthX = compArea.width / numPartX;
    int lengthY = compArea.height / numPartY;
    int strideOrg  = orgPel.stride;
    int stridePred = predPel.stride;
    uint32_t          shift = DISTORTION_PRECISION_ADJUSTMENT((*cs.sps.getBitDepth(toChannelType(compID)) - 8) << 1);
    Intermediate_Int  temp;

    //calc distY of 16 sub parts
    for( int j = 0; j < numPartY; j++ )
    {
      for( int i = 0; i < numPartX; i++ )
      {
        int posX = i * lengthX;
        int posY = j * lengthY;
        const Pel* ptrOrg  = orgPel.bufAt( posX, posY );
        const Pel* ptrPred = predPel.bufAt( posX, posY );
        Distortion sum     = 0;
        for( int n = 0; n < lengthY; n++ )
        {
          for( int m = 0; m < lengthX; m++ )
          {
            temp = ptrOrg[m] - ptrPred[m];
            sum += Distortion((temp * temp) >> shift);
          }
          ptrOrg += strideOrg;
          ptrPred += stridePred;
        }
        if( isChroma( compID ) )
        {
          sum = (Distortion) (sum * m_pcRdCost->getChromaWeight());
        }
        dist[j][i] += sum;
      }
    }
  }

  //SSE of a CU
  m_estMinDistSbt[NUMBER_SBT_MODE] = 0;
  for( int j = 0; j < numPartY; j++ )
  {
    for( int i = 0; i < numPartX; i++ )
    {
      m_estMinDistSbt[NUMBER_SBT_MODE] += dist[j][i];
    }
  }
  //init per-mode dist
  for( int i = SBT_VER_H0; i < NUMBER_SBT_MODE; i++ )
  {
    m_estMinDistSbt[i] = std::numeric_limits<uint64_t>::max();
  }

  //SBT fast algorithm 1: not try SBT if the residual is too small to compensate bits for encoding residual info
  uint64_t minNonZeroResiFracBits = 12 << SCALE_BITS;
  if( m_pcRdCost->calcRdCost( 0, m_estMinDistSbt[NUMBER_SBT_MODE] ) < m_pcRdCost->calcRdCost( minNonZeroResiFracBits, 0 ) )
  {
    m_skipSbtAll = true;
    return;
  }

  //derive estimated minDist of SBT = zero-residual part distortion + non-zero residual part distortion / 16
  int shift = 5;
  Distortion distResiPart = 0, distNoResiPart = 0;

  if( CU::targetSbtAllowed( SBT_VER_HALF, sbtAllowed ) )
  {
    int offsetResiPart = 0;
    int offsetNoResiPart = numPartX / 2;
    distResiPart = distNoResiPart = 0;
    assert( numPartX >= 2 );
    for( int j = 0; j < numPartY; j++ )
    {
      for( int i = 0; i < numPartX / 2; i++ )
      {
        distResiPart   += dist[j][i + offsetResiPart];
        distNoResiPart += dist[j][i + offsetNoResiPart];
      }
    }
    m_estMinDistSbt[SBT_VER_H0] = ( distResiPart >> shift ) + distNoResiPart;
    m_estMinDistSbt[SBT_VER_H1] = ( distNoResiPart >> shift ) + distResiPart;
  }

  if( CU::targetSbtAllowed( SBT_HOR_HALF, sbtAllowed ) )
  {
    int offsetResiPart = 0;
    int offsetNoResiPart = numPartY / 2;
    assert( numPartY >= 2 );
    distResiPart = distNoResiPart = 0;
    for( int j = 0; j < numPartY / 2; j++ )
    {
      for( int i = 0; i < numPartX; i++ )
      {
        distResiPart   += dist[j + offsetResiPart][i];
        distNoResiPart += dist[j + offsetNoResiPart][i];
      }
    }
    m_estMinDistSbt[SBT_HOR_H0] = ( distResiPart >> shift ) + distNoResiPart;
    m_estMinDistSbt[SBT_HOR_H1] = ( distNoResiPart >> shift ) + distResiPart;
  }

  if( CU::targetSbtAllowed( SBT_VER_QUAD, sbtAllowed ) )
  {
    assert( numPartX == 4 );
    m_estMinDistSbt[SBT_VER_Q0] = m_estMinDistSbt[SBT_VER_Q1] = 0;
    for( int j = 0; j < numPartY; j++ )
    {
      m_estMinDistSbt[SBT_VER_Q0] += dist[j][0] + ( ( dist[j][1] + dist[j][2] + dist[j][3] ) << shift );
      m_estMinDistSbt[SBT_VER_Q1] += dist[j][3] + ( ( dist[j][0] + dist[j][1] + dist[j][2] ) << shift );
    }
    m_estMinDistSbt[SBT_VER_Q0] = m_estMinDistSbt[SBT_VER_Q0] >> shift;
    m_estMinDistSbt[SBT_VER_Q1] = m_estMinDistSbt[SBT_VER_Q1] >> shift;
  }

  if( CU::targetSbtAllowed( SBT_HOR_QUAD, sbtAllowed ) )
  {
    assert( numPartY == 4 );
    m_estMinDistSbt[SBT_HOR_Q0] = m_estMinDistSbt[SBT_HOR_Q1] = 0;
    for( int i = 0; i < numPartX; i++ )
    {
      m_estMinDistSbt[SBT_HOR_Q0] += dist[0][i] + ( ( dist[1][i] + dist[2][i] + dist[3][i] ) << shift );
      m_estMinDistSbt[SBT_HOR_Q1] += dist[3][i] + ( ( dist[0][i] + dist[1][i] + dist[2][i] ) << shift );
    }
    m_estMinDistSbt[SBT_HOR_Q0] = m_estMinDistSbt[SBT_HOR_Q0] >> shift;
    m_estMinDistSbt[SBT_HOR_Q1] = m_estMinDistSbt[SBT_HOR_Q1] >> shift;
  }

  //SBT fast algorithm 5: try N SBT modes with the lowest distortion
  Distortion temp[NUMBER_SBT_MODE];
  memcpy( temp, m_estMinDistSbt, sizeof( Distortion ) * NUMBER_SBT_MODE );
  memset( m_sbtRdoOrder, 255, NUMBER_SBT_MODE );
  int startIdx = 0, numRDO;
  numRDO = CU::targetSbtAllowed( SBT_VER_HALF, sbtAllowed ) + CU::targetSbtAllowed( SBT_HOR_HALF, sbtAllowed );
  numRDO = std::min( ( numRDO << 1 ), SBT_NUM_RDO );
  for( int i = startIdx; i < startIdx + numRDO; i++ )
  {
    Distortion minDist = std::numeric_limits<uint64_t>::max();
    for( int n = SBT_VER_H0; n <= SBT_HOR_H1; n++ )
    {
      if( temp[n] < minDist )
      {
        minDist = temp[n];
        m_sbtRdoOrder[i] = n;
      }
    }
    temp[m_sbtRdoOrder[i]] = std::numeric_limits<uint64_t>::max();
  }

  startIdx += numRDO;
  numRDO = CU::targetSbtAllowed( SBT_VER_QUAD, sbtAllowed ) + CU::targetSbtAllowed( SBT_HOR_QUAD, sbtAllowed );
  numRDO = std::min( ( numRDO << 1 ), SBT_NUM_RDO );
  for( int i = startIdx; i < startIdx + numRDO; i++ )
  {
    Distortion minDist = std::numeric_limits<uint64_t>::max();
    for( int n = SBT_VER_Q0; n <= SBT_HOR_Q1; n++ )
    {
      if( temp[n] < minDist )
      {
        minDist = temp[n];
        m_sbtRdoOrder[i] = n;
      }
    }
    temp[m_sbtRdoOrder[i]] = std::numeric_limits<uint64_t>::max();
  }
}

uint8_t InterSearch::skipSbtByRDCost( int width, int height, int mtDepth, uint8_t sbtIdx, uint8_t sbtPos, double bestCost, Distortion distSbtOff, double costSbtOff, bool rootCbfSbtOff )
{
  int sbtMode = CU::getSbtMode( sbtIdx, sbtPos );

  //SBT fast algorithm 2.2 : estimate a minimum RD cost of a SBT mode based on the luma distortion of uncoded part and coded part (assuming distorted can be reduced to 1/16);
  //                         if this cost is larger than the best cost, no need to try a specific SBT mode
  if( m_pcRdCost->calcRdCost( 11 << SCALE_BITS, m_estMinDistSbt[sbtMode] ) > bestCost )
  {
    return 0; //early skip type 0
  }

  if( costSbtOff != MAX_DOUBLE )
  {
    if( !rootCbfSbtOff )
    {
      //SBT fast algorithm 3: skip SBT when the residual is too small (estCost is more accurate than fast algorithm 1, counting PU mode bits)
      uint64_t minNonZeroResiFracBits = 10 << SCALE_BITS;
      Distortion distResiPart;
      if( sbtIdx == SBT_VER_HALF || sbtIdx == SBT_HOR_HALF )
      {
        distResiPart = (Distortion)( ( ( m_estMinDistSbt[NUMBER_SBT_MODE] - m_estMinDistSbt[sbtMode] ) * 9 ) >> 4 );
      }
      else
      {
        distResiPart = (Distortion)( ( ( m_estMinDistSbt[NUMBER_SBT_MODE] - m_estMinDistSbt[sbtMode] ) * 3 ) >> 3 );
      }

      double estCost = ( costSbtOff - m_pcRdCost->calcRdCost( 0 << SCALE_BITS, distSbtOff ) ) + m_pcRdCost->calcRdCost( minNonZeroResiFracBits, m_estMinDistSbt[sbtMode] + distResiPart );
      if( estCost > costSbtOff )
      {
        return 1;
      }
      if( estCost > bestCost )
      {
        return 2;
      }
    }
    else
    {
      //SBT fast algorithm 4: skip SBT when an estimated RD cost is larger than the bestCost
      double weight = sbtMode > SBT_HOR_H1 ? 0.4 : 0.6;
      double estCost = ( ( costSbtOff - m_pcRdCost->calcRdCost( 0 << SCALE_BITS, distSbtOff ) ) * weight ) + m_pcRdCost->calcRdCost( 0 << SCALE_BITS, m_estMinDistSbt[sbtMode] );
      if( estCost > bestCost )
      {
        return 3;
      }
    }
  }
  return MAX_UCHAR;
}

void InterSearch::xEstimateInterResidualQT(CodingStructure &cs, Partitioner &partitioner,
                                           Distortion *puiZeroDist /*= nullptr*/
                                           ,
                                           const bool luma, const bool chroma, PelUnitBuf *orgResi)
{
  const UnitArea& currArea = partitioner.currArea();
  const SPS &sps           = *cs.sps;
  m_pcRdCost->setChromaFormat(sps.getChromaFormatIdc());

  const uint32_t numValidComp  = getNumberValidComponents( sps.getChromaFormatIdc() );
  const uint32_t numTBlocks    = getNumberValidTBlocks   ( *cs.pcv );
  const CodingUnit &cu = *cs.getCU(partitioner.chType);
  const unsigned currDepth = partitioner.currTrDepth;
  const bool colorTransFlag = cs.cus[0]->colorTransform;

  bool bCheckFull  = !partitioner.canSplit( TU_MAX_TR_SPLIT, cs );
  if( cu.sbtInfo && partitioner.canSplit( PartSplit( cu.getSbtTuSplit() ), cs ) )
  {
    bCheckFull = false;
  }
  bool bCheckSplit = !bCheckFull;

  // get temporary data
  CodingStructure *csSplit = nullptr;
  CodingStructure *csFull  = nullptr;
  if (bCheckSplit)
  {
    csSplit = &cs;
  }
  else if (bCheckFull)
  {
    csFull = &cs;
  }

  Distortion uiSingleDist         = 0;
  Distortion uiSingleDistComp [3] = { 0, 0, 0 };
  uint64_t   uiSingleFracBits[3] = { 0, 0, 0 };

  const TempCtx ctxStart  ( m_CtxCache, m_CABACEstimator->getCtx() );
  TempCtx       ctxBest   ( m_CtxCache );

  if (bCheckFull)
  {
    TransformUnit &tu = csFull->addTU(CS::getArea(cs, currArea, partitioner.chType), partitioner.chType);
    tu.depth          = currDepth;
    for (int i = 0; i<MAX_NUM_TBLOCKS; i++) tu.mtsIdx[i] = MTS_DCT2_DCT2;
    tu.checkTuNoResidual( partitioner.currPartIdx() );
    Position tuPos = tu.Y();
    tuPos.relativeTo(cu.Y());
    const UnitArea relativeUnitArea(tu.chromaFormat, Area(tuPos, tu.Y().size()));

    const Slice           &slice = *cs.slice;
    if (slice.getLmcsEnabledFlag() && slice.getPicHeader()->getLmcsChromaResidualScaleFlag() && !(CS::isDualITree(cs) && slice.isIntra() && tu.cu->predMode == MODE_IBC))
    {
      const CompArea      &areaY = tu.blocks[COMPONENT_Y];
      int adj = m_pcReshape->calculateChromaAdjVpduNei(tu, areaY);
      tu.setChromaAdj(adj);
    }

    PelUnitBuf colorTransResidual = m_colorTransResiBuf[1].getBuf(relativeUnitArea);
    if (colorTransFlag)
    {
      csFull->getResiBuf(currArea).copyFrom(cs.getOrgResiBuf(currArea));
      if (slice.getLmcsEnabledFlag() && slice.getPicHeader()->getLmcsChromaResidualScaleFlag() && tu.blocks[COMPONENT_Cb].width*tu.blocks[COMPONENT_Cr].height > 4)
      {
        csFull->getResiBuf(currArea).bufs[1].scaleSignal(tu.getChromaAdj(), 1, tu.cu->cs->slice->clpRng(COMPONENT_Cb));
        csFull->getResiBuf(currArea).bufs[2].scaleSignal(tu.getChromaAdj(), 1, tu.cu->cs->slice->clpRng(COMPONENT_Cr));
      }
      csFull->getResiBuf(currArea).colorSpaceConvert(colorTransResidual, true, cu.cs->slice->clpRng(COMPONENT_Y));
    }
    double minCost            [MAX_NUM_TBLOCKS];

    m_CABACEstimator->resetBits();

    memset(m_pTempPel, 0, sizeof(Pel) * tu.Y().area()); // not necessary needed for inside of recursion (only at the beginning)

    for (uint32_t i = 0; i < numTBlocks; i++)
    {
      minCost[i] = MAX_DOUBLE;
    }

    CodingStructure &saveCS = *m_pSaveCS[0];
    saveCS.pcv     = cs.pcv;
    saveCS.picture = cs.picture;
    saveCS.area.repositionTo(currArea);
    saveCS.clearTUs();
    TransformUnit & bestTU = saveCS.addTU(CS::getArea(cs, currArea, partitioner.chType), partitioner.chType);

    for( uint32_t c = 0; c < numTBlocks; c++ )
    {
      const ComponentID compID    = ComponentID(c);
      if (compID == COMPONENT_Y && !luma)
      {
        continue;
      }
      if (compID != COMPONENT_Y && !chroma)
      {
        continue;
      }
      const CompArea&   compArea  = tu.blocks[compID];
      const int channelBitDepth   = sps.getBitDepth(toChannelType(compID));

      if( !tu.blocks[compID].valid() )
      {
        continue;
      }


      const bool tsAllowed  = TU::isTSAllowed(tu, compID) && (isLuma(compID) || (isChroma(compID) && m_pcEncCfg->getUseChromaTS()));
      const bool mtsAllowed = CU::isMTSAllowed( *tu.cu, compID );

      uint8_t nNumTransformCands = 1 + ( tsAllowed ? 1 : 0 ) + ( mtsAllowed ? 4 : 0 ); // DCT + TS + 4 MTS = 6 tests
      std::vector<TrMode> trModes;
      if (m_pcEncCfg->getCostMode() == COST_LOSSLESS_CODING && slice.isLossless())
      {
        nNumTransformCands = 0;
      }
      else
      {
        trModes.push_back(TrMode(0, true));   // DCT2
        nNumTransformCands = 1;
      }
      //for a SBT-no-residual TU, the RDO process should be called once, in order to get the RD cost
      if( tsAllowed && !tu.noResidual )
      {
        trModes.push_back( TrMode( 1, true ) );
        nNumTransformCands++;
      }

#if APPLY_SBT_SL_ON_MTS
      //skip MTS if DCT2 is the best
      if( mtsAllowed && ( !tu.cu->slice->getSPS()->getUseSBT() || CU::getSbtIdx( m_histBestSbt ) != SBT_OFF_DCT ) )
#else
      if( mtsAllowed )
#endif
      {
        for( int i = 2; i < 6; i++ )
        {
#if APPLY_SBT_SL_ON_MTS
          //skip the non-best Mts mode
          if( !tu.cu->slice->getSPS()->getUseSBT() || ( m_histBestMtsIdx == MAX_UCHAR || m_histBestMtsIdx == i ) )
          {
#endif
          trModes.push_back( TrMode( i, true ) );
          nNumTransformCands++;
#if APPLY_SBT_SL_ON_MTS
          }
#endif
        }
      }
      if (colorTransFlag && (m_pcEncCfg->getCostMode() != COST_LOSSLESS_CODING || !slice.isLossless()))
      {
        m_pcTrQuant->lambdaAdjustColorTrans(true);
        if (isChroma(compID) && slice.getLmcsEnabledFlag() && slice.getPicHeader()->getLmcsChromaResidualScaleFlag() && tu.blocks[compID].width*tu.blocks[compID].height > 4)
        {
          int cResScaleInv = tu.getChromaAdj();
          m_pcRdCost->lambdaAdjustColorTrans(true, compID, true, &cResScaleInv);
        }
        else
        m_pcRdCost->lambdaAdjustColorTrans(true, compID);
      }

      const int numTransformCandidates = nNumTransformCands;
      for( int transformMode = 0; transformMode < numTransformCandidates; transformMode++ )
      {
        const bool isFirstMode = transformMode == 0;
        // copy the original residual into the residual buffer
        if (colorTransFlag)
        {
          csFull->getResiBuf(compArea).copyFrom(colorTransResidual.bufs[compID]);
        }
        else
          csFull->getResiBuf(compArea).copyFrom(cs.getOrgResiBuf(compArea));

        m_CABACEstimator->getCtx() = ctxStart;
        m_CABACEstimator->resetBits();

        if (!(m_pcEncCfg->getCostMode() == COST_LOSSLESS_CODING && slice.isLossless()))
        {
          if (bestTU.mtsIdx[compID] == MTS_SKIP && m_pcEncCfg->getUseTransformSkipFast())
          {
            continue;
          }
          if (!trModes[transformMode].second)
          {
            continue;
          }
        }
        tu.mtsIdx[compID] = trModes[transformMode].first;
        QpParam cQP(tu, compID);   // note: uses tu.transformSkip[compID]

#if RDOQ_CHROMA_LAMBDA
        m_pcTrQuant->selectLambda(compID);
#endif
        if (slice.getLmcsEnabledFlag() && isChroma(compID) && slice.getPicHeader()->getLmcsChromaResidualScaleFlag())
        {
          double cRescale = (double) (1 << CSCALE_FP_PREC) / (double) (tu.getChromaAdj());
          m_pcTrQuant->setLambda(m_pcTrQuant->getLambda() / (cRescale * cRescale));
        }
        if (sps.getJointCbCrEnabledFlag() && isChroma(compID) && (tu.cu->cs->slice->getSliceQp() > 18))
        {
          m_pcTrQuant->setLambda(1.05 * m_pcTrQuant->getLambda());
        }

        TCoeff     currAbsSum       = 0;
        uint64_t   currCompFracBits = 0;
        Distortion currCompDist     = 0;
        double     currCompCost     = 0;
        uint64_t   nonCoeffFracBits = 0;
        Distortion nonCoeffDist     = 0;
        double     nonCoeffCost     = 0;

        if (!colorTransFlag && slice.getLmcsEnabledFlag() && isChroma(compID)
            && slice.getPicHeader()->getLmcsChromaResidualScaleFlag()
            && tu.blocks[compID].width * tu.blocks[compID].height > 4)
        {
          PelBuf resiBuf = csFull->getResiBuf(compArea);
          resiBuf.scaleSignal(tu.getChromaAdj(), 1, tu.cu->cs->slice->clpRng(compID));
        }
        if (nNumTransformCands > 1)
        {
          if (transformMode == 0)
          {
            m_pcTrQuant->transformNxN(tu, compID, cQP, &trModes, m_pcEncCfg->getMTSInterMaxCand());
            tu.mtsIdx[compID] = trModes[0].first;
          }
          if (!(m_pcEncCfg->getCostMode() == COST_LOSSLESS_CODING && slice.isLossless() && tu.mtsIdx[compID] == 0))
          {
            m_pcTrQuant->transformNxN(tu, compID, cQP, currAbsSum, m_CABACEstimator->getCtx(), true);
          }
        }
        else
        {
          m_pcTrQuant->transformNxN(tu, compID, cQP, currAbsSum, m_CABACEstimator->getCtx());
        }

        if (isFirstMode || (currAbsSum == 0))
        {
          const CPelBuf zeroBuf(m_pTempPel, compArea);
          const CPelBuf orgResi = colorTransFlag ? colorTransResidual.bufs[compID] : csFull->getOrgResiBuf(compArea);

          {
            nonCoeffDist = m_pcRdCost->getDistPart(zeroBuf, orgResi, channelBitDepth, compID,
                                                   DF_SSE);   // initialized with zero residual distortion
          }

          if (!tu.noResidual)
          {
            const bool prevCbf = ( compID == COMPONENT_Cr ? tu.cbf[COMPONENT_Cb] : false );
            m_CABACEstimator->cbf_comp( *csFull, false, compArea, currDepth, prevCbf );
          }

          nonCoeffFracBits = m_CABACEstimator->getEstFracBits();
#if WCG_EXT
          if (m_pcEncCfg->getLumaLevelToDeltaQPMapping().isEnabled())
          {
            nonCoeffCost = m_pcRdCost->calcRdCost(nonCoeffFracBits, nonCoeffDist, false);
          }
          else
#endif
            if (cs.slice->getSPS()->getUseColorTrans())
          {
            nonCoeffCost = m_pcRdCost->calcRdCost(nonCoeffFracBits, nonCoeffDist, false);
          }
          else
          {
            nonCoeffCost = m_pcRdCost->calcRdCost(nonCoeffFracBits, nonCoeffDist);
          }
        }

        if ((puiZeroDist != nullptr) && isFirstMode)
        {
          *puiZeroDist += nonCoeffDist;   // initialized with zero residual distortion
        }
        if (m_pcEncCfg->getCostMode() == COST_LOSSLESS_CODING && slice.isLossless() && tu.mtsIdx[compID] == 0)
        {
          currAbsSum = 0;
        }

        if (currAbsSum
            > 0)   // if non-zero coefficients are present, a residual needs to be derived for further prediction
        {
          if (isFirstMode)
          {
            m_CABACEstimator->getCtx() = ctxStart;
            m_CABACEstimator->resetBits();
          }

          const bool prevCbf = (compID == COMPONENT_Cr ? tu.cbf[COMPONENT_Cb] : false);
          m_CABACEstimator->cbf_comp(*csFull, true, compArea, currDepth, prevCbf);
          if (compID == COMPONENT_Cr)
          {
            const int cbfMask = (tu.cbf[COMPONENT_Cb] ? 2 : 0) + 1;
            m_CABACEstimator->joint_cb_cr(tu, cbfMask);
          }

          CUCtx cuCtx;
          cuCtx.isDQPCoded         = true;
          cuCtx.isChromaQpAdjCoded = true;
          m_CABACEstimator->residual_coding(tu, compID, &cuCtx);
          m_CABACEstimator->mts_idx(cu, &cuCtx);

          if (compID == COMPONENT_Y && tu.mtsIdx[compID] > MTS_SKIP && !cuCtx.mtsLastScanPos)
          {
            currCompCost = MAX_DOUBLE;
          }
          else
          {
            currCompFracBits = m_CABACEstimator->getEstFracBits();

            PelBuf resiBuf = csFull->getResiBuf(compArea);
            CPelBuf orgResiBuf = colorTransFlag ? colorTransResidual.bufs[compID] : csFull->getOrgResiBuf(compArea);

            m_pcTrQuant->invTransformNxN(tu, compID, resiBuf, cQP);
            if (!colorTransFlag && slice.getLmcsEnabledFlag() && isChroma(compID) && slice.getPicHeader()->getLmcsChromaResidualScaleFlag() && tu.blocks[compID].width*tu.blocks[compID].height > 4)
            {
              resiBuf.scaleSignal(tu.getChromaAdj(), 0, tu.cu->cs->slice->clpRng(compID));
            }

            currCompDist = m_pcRdCost->getDistPart(orgResiBuf, resiBuf, channelBitDepth, compID, DF_SSE);

#if WCG_EXT
            currCompCost = m_pcRdCost->calcRdCost(currCompFracBits, currCompDist, false);
#else
            currCompCost = m_pcRdCost->calcRdCost(currCompFracBits, currCompDist);
#endif
          }
        }
        else if (transformMode > 0)
        {
          currCompCost = MAX_DOUBLE;
        }
        else
        {
          currCompFracBits = nonCoeffFracBits;
          currCompDist     = nonCoeffDist;
          currCompCost     = nonCoeffCost;

          tu.cbf[compID] = 0;
        }

        // evaluate
        if ((currCompCost < minCost[compID]) || (transformMode == 1 && currCompCost == minCost[compID]))
        {
          // copy component
          if (isFirstMode && ((nonCoeffCost < currCompCost) || (currAbsSum == 0)))   // check for forced null
          {
            tu.getCoeffs(compID).fill(0);
            csFull->getResiBuf(compArea).fill(0);
            tu.cbf[compID] = 0;

            currAbsSum       = 0;
            currCompFracBits = nonCoeffFracBits;
            currCompDist     = nonCoeffDist;
            currCompCost     = nonCoeffCost;
          }

          uiSingleDistComp[compID] = currCompDist;
          uiSingleFracBits[compID] = currCompFracBits;
          minCost[compID]          = currCompCost;

          bestTU.copyComponentFrom(tu, compID);
          saveCS.getResiBuf(compArea).copyFrom(csFull->getResiBuf(compArea));
        }
        if (tu.noResidual)
        {
          CHECK(currCompFracBits > 0 || currAbsSum, "currCompFracBits > 0 when tu noResidual");
        }
      }

      // copy component
      tu.copyComponentFrom(bestTU, compID);
      csFull->getResiBuf(compArea).copyFrom(saveCS.getResiBuf(compArea));
      if (colorTransFlag && (m_pcEncCfg->getCostMode() != COST_LOSSLESS_CODING || !slice.isLossless()))
      {
        m_pcTrQuant->lambdaAdjustColorTrans(false);
        m_pcRdCost->lambdaAdjustColorTrans(false, compID);
      }

    } // component loop

    if (colorTransFlag)
    {
      PelUnitBuf     orgResidual = orgResi->subBuf(relativeUnitArea);
      PelUnitBuf     invColorTransResidual = m_colorTransResiBuf[2].getBuf(relativeUnitArea);
      csFull->getResiBuf(currArea).colorSpaceConvert(invColorTransResidual, false, slice.clpRng(COMPONENT_Y));
      if (slice.getLmcsEnabledFlag() && slice.getPicHeader()->getLmcsChromaResidualScaleFlag() && tu.blocks[COMPONENT_Cb].width*tu.blocks[COMPONENT_Cb].height > 4)
      {
        invColorTransResidual.bufs[1].scaleSignal(tu.getChromaAdj(), 0, tu.cu->cs->slice->clpRng(COMPONENT_Cb));
        invColorTransResidual.bufs[2].scaleSignal(tu.getChromaAdj(), 0, tu.cu->cs->slice->clpRng(COMPONENT_Cr));
      }

      for (uint32_t c = 0; c < numTBlocks; c++)
      {
        const ComponentID compID = (ComponentID)c;
        uiSingleDistComp[c] = m_pcRdCost->getDistPart(orgResidual.bufs[c], invColorTransResidual.bufs[c], sps.getBitDepth(toChannelType(compID)), compID, DF_SSE);
        minCost[c] = m_pcRdCost->calcRdCost(uiSingleFracBits[c], uiSingleDistComp[c]);
      }
    }

    if ( chroma && isChromaEnabled(tu.chromaFormat) && tu.blocks[COMPONENT_Cb].valid() )
    {
      const CompArea& cbArea = tu.blocks[COMPONENT_Cb];
      const CompArea& crArea = tu.blocks[COMPONENT_Cr];
      bool checkJointCbCr = (sps.getJointCbCrEnabledFlag()) && (!tu.noResidual) && (TU::getCbf(tu, COMPONENT_Cb) || TU::getCbf(tu, COMPONENT_Cr));
      bool checkDCTOnly = (TU::getCbf(tu, COMPONENT_Cb) && tu.mtsIdx[COMPONENT_Cb] == MTS_DCT2_DCT2 && !TU::getCbf(tu, COMPONENT_Cr)) ||
                          (TU::getCbf(tu, COMPONENT_Cr) && tu.mtsIdx[COMPONENT_Cr] == MTS_DCT2_DCT2 && !TU::getCbf(tu, COMPONENT_Cb)) ||
                          (TU::getCbf(tu, COMPONENT_Cb) && tu.mtsIdx[COMPONENT_Cb] == MTS_DCT2_DCT2 && TU::getCbf(tu, COMPONENT_Cr) && tu.mtsIdx[COMPONENT_Cr] == MTS_DCT2_DCT2);

      bool checkTSOnly = (TU::getCbf(tu, COMPONENT_Cb) && tu.mtsIdx[COMPONENT_Cb] == MTS_SKIP && !TU::getCbf(tu, COMPONENT_Cr)) ||
                         (TU::getCbf(tu, COMPONENT_Cr) && tu.mtsIdx[COMPONENT_Cr] == MTS_SKIP && !TU::getCbf(tu, COMPONENT_Cb)) ||
                         (TU::getCbf(tu, COMPONENT_Cb) && tu.mtsIdx[COMPONENT_Cb] == MTS_SKIP && TU::getCbf(tu, COMPONENT_Cr) && tu.mtsIdx[COMPONENT_Cr] == MTS_SKIP);
      const int channelBitDepth = sps.getBitDepth(toChannelType(COMPONENT_Cb));
      bool      reshape         = slice.getLmcsEnabledFlag() && slice.getPicHeader()->getLmcsChromaResidualScaleFlag()
                               && tu.blocks[COMPONENT_Cb].width * tu.blocks[COMPONENT_Cb].height > 4;
      double minCostCbCr = minCost[COMPONENT_Cb] + minCost[COMPONENT_Cr];
      if (colorTransFlag)
      {
        minCostCbCr += minCost[COMPONENT_Y];  // ACT should consider three-component cost
      }

      CompStorage      orgResiCb[4], orgResiCr[4];   // 0:std, 1-3:jointCbCr
      std::vector<int> jointCbfMasksToTest;
      if ( checkJointCbCr )
      {
        orgResiCb[0].create(cbArea);
        orgResiCr[0].create(crArea);
        if (colorTransFlag)
        {
          orgResiCb[0].copyFrom(colorTransResidual.bufs[1]);
          orgResiCr[0].copyFrom(colorTransResidual.bufs[2]);
        }
        else
        {
          orgResiCb[0].copyFrom(cs.getOrgResiBuf(cbArea));
          orgResiCr[0].copyFrom(cs.getOrgResiBuf(crArea));
        }
        if (!colorTransFlag && reshape)
        {
          orgResiCb[0].scaleSignal(tu.getChromaAdj(), 1, tu.cu->cs->slice->clpRng(COMPONENT_Cb));
          orgResiCr[0].scaleSignal(tu.getChromaAdj(), 1, tu.cu->cs->slice->clpRng(COMPONENT_Cr));
        }
        jointCbfMasksToTest = m_pcTrQuant->selectICTCandidates(tu, orgResiCb, orgResiCr);
      }

      for (int cbfMask: jointCbfMasksToTest)
      {
        ComponentID codeCompId = (cbfMask >> 1 ? COMPONENT_Cb : COMPONENT_Cr);
        ComponentID otherCompId = (codeCompId == COMPONENT_Cr ? COMPONENT_Cb : COMPONENT_Cr);
        bool        tsAllowed = TU::isTSAllowed(tu, codeCompId) && (m_pcEncCfg->getUseChromaTS());
        uint8_t     numTransformCands = 1 + (tsAllowed ? 1 : 0); // DCT + TS = 2 tests
        bool        cbfDCT2 = true;

        std::vector<TrMode> trModes;
        if (checkDCTOnly || checkTSOnly)
        {
          numTransformCands = 1;
        }

        if (!checkTSOnly)
        {
          trModes.push_back(TrMode(0, true)); // DCT2
        }
        if (tsAllowed && !checkDCTOnly)
        {
          trModes.push_back(TrMode(1, true));//TS
        }
        for (int modeId = 0; modeId < numTransformCands; modeId++)
        {
          if (modeId && !cbfDCT2)
          {
            continue;
          }
          if (!trModes[modeId].second)
          {
            continue;
          }
          TCoeff     currAbsSum       = 0;
          uint64_t   currCompFracBits = 0;
          Distortion currCompDistCb   = 0;
          Distortion currCompDistCr   = 0;
          double     currCompCost     = 0;

          tu.jointCbCr = (uint8_t) cbfMask;
          // encoder bugfix: initialize mtsIdx for chroma under JointCbCrMode.
          tu.mtsIdx[codeCompId]  = trModes[modeId].first;
          tu.mtsIdx[otherCompId] = MTS_DCT2_DCT2;
          int codedCbfMask       = 0;
          if (colorTransFlag && (m_pcEncCfg->getCostMode() != COST_LOSSLESS_CODING || !slice.isLossless()))
          {
            m_pcTrQuant->lambdaAdjustColorTrans(true);
            m_pcTrQuant->selectLambda(codeCompId);
          }
          else
          {
            m_pcTrQuant->selectLambda(codeCompId);
          }
          // Lambda is loosened for the joint mode with respect to single modes as the same residual is used for both
          // chroma blocks
          const int    absIct = abs(TU::getICTMode(tu));
          const double lfact  = (absIct == 1 || absIct == 3 ? 0.8 : 0.5);
          m_pcTrQuant->setLambda(lfact * m_pcTrQuant->getLambda());
          if (checkJointCbCr && (tu.cu->cs->slice->getSliceQp() > 18))
          {
            m_pcTrQuant->setLambda(1.05 * m_pcTrQuant->getLambda());
          }

          m_CABACEstimator->getCtx() = ctxStart;
          m_CABACEstimator->resetBits();

          PelBuf cbResi = csFull->getResiBuf(cbArea);
          PelBuf crResi = csFull->getResiBuf(crArea);
          cbResi.copyFrom(orgResiCb[cbfMask]);
          crResi.copyFrom(orgResiCr[cbfMask]);

          if (reshape)
          {
            double cRescale = (double) (1 << CSCALE_FP_PREC) / (double) (tu.getChromaAdj());
            m_pcTrQuant->setLambda(m_pcTrQuant->getLambda() / (cRescale * cRescale));
          }

          Distortion currCompDistY = MAX_UINT64;
          QpParam    qpCbCr(tu, codeCompId);

          tu.getCoeffs(otherCompId).fill(0);   // do we need that?
          TU::setCbfAtDepth(tu, otherCompId, tu.depth, false);

          PelBuf &codeResi   = (codeCompId == COMPONENT_Cr ? crResi : cbResi);
          TCoeff  compAbsSum = 0;
          if (numTransformCands > 1)
          {
            if (modeId == 0)
            {
              m_pcTrQuant->transformNxN(tu, codeCompId, qpCbCr, &trModes, m_pcEncCfg->getMTSInterMaxCand());
              tu.mtsIdx[codeCompId]  = trModes[modeId].first;
              tu.mtsIdx[otherCompId] = MTS_DCT2_DCT2;
            }
            m_pcTrQuant->transformNxN(tu, codeCompId, qpCbCr, compAbsSum, m_CABACEstimator->getCtx(), true);
          }
          else
          {
            m_pcTrQuant->transformNxN(tu, codeCompId, qpCbCr, compAbsSum, m_CABACEstimator->getCtx());
          }
          if (compAbsSum > 0)
          {
            m_pcTrQuant->invTransformNxN(tu, codeCompId, codeResi, qpCbCr);
            codedCbfMask += (codeCompId == COMPONENT_Cb ? 2 : 1);
          }
          else
          {
            codeResi.fill(0);
          }

          if (tu.jointCbCr == 3 && codedCbfMask == 2)
          {
            codedCbfMask = 3;
            TU::setCbfAtDepth(tu, COMPONENT_Cr, tu.depth, true);
          }
          if (codedCbfMask && tu.jointCbCr != codedCbfMask)
          {
            codedCbfMask = 0;
          }
          currAbsSum = codedCbfMask;

          if (!tu.mtsIdx[codeCompId])
          {
            cbfDCT2 = (currAbsSum > 0);
          }
          if (currAbsSum > 0)
          {
            m_CABACEstimator->cbf_comp(cs, codedCbfMask >> 1, cbArea, currDepth, false);
            m_CABACEstimator->cbf_comp(cs, codedCbfMask & 1, crArea, currDepth, codedCbfMask >> 1);
            m_CABACEstimator->joint_cb_cr(tu, codedCbfMask);
            if (codedCbfMask >> 1)
            {
              m_CABACEstimator->residual_coding(tu, COMPONENT_Cb);
            }
            if (codedCbfMask & 1)
            {
              m_CABACEstimator->residual_coding(tu, COMPONENT_Cr);
            }
            currCompFracBits = m_CABACEstimator->getEstFracBits();

            m_pcTrQuant->invTransformICT(tu, cbResi, crResi);
            if (!colorTransFlag && reshape)
            {
              cbResi.scaleSignal(tu.getChromaAdj(), 0, tu.cu->cs->slice->clpRng(COMPONENT_Cb));
              crResi.scaleSignal(tu.getChromaAdj(), 0, tu.cu->cs->slice->clpRng(COMPONENT_Cr));
            }

            if (colorTransFlag)
            {
              PelUnitBuf orgResidual           = orgResi->subBuf(relativeUnitArea);
              PelUnitBuf invColorTransResidual = m_colorTransResiBuf[2].getBuf(relativeUnitArea);
              csFull->getResiBuf(currArea).colorSpaceConvert(invColorTransResidual, false, slice.clpRng(COMPONENT_Y));
              if (reshape)
              {
                invColorTransResidual.bufs[1].scaleSignal(tu.getChromaAdj(), 0, tu.cu->cs->slice->clpRng(COMPONENT_Cb));
                invColorTransResidual.bufs[2].scaleSignal(tu.getChromaAdj(), 0, tu.cu->cs->slice->clpRng(COMPONENT_Cr));
              }

              currCompDistY =
                m_pcRdCost->getDistPart(orgResidual.bufs[COMPONENT_Y], invColorTransResidual.bufs[COMPONENT_Y],
                                        sps.getBitDepth(toChannelType(COMPONENT_Y)), COMPONENT_Y, DF_SSE);
              currCompDistCb =
                m_pcRdCost->getDistPart(orgResidual.bufs[COMPONENT_Cb], invColorTransResidual.bufs[COMPONENT_Cb],
                                        sps.getBitDepth(toChannelType(COMPONENT_Cb)), COMPONENT_Cb, DF_SSE);
              currCompDistCr =
                m_pcRdCost->getDistPart(orgResidual.bufs[COMPONENT_Cr], invColorTransResidual.bufs[COMPONENT_Cr],
                                        sps.getBitDepth(toChannelType(COMPONENT_Cr)), COMPONENT_Cr, DF_SSE);
              currCompCost = m_pcRdCost->calcRdCost(uiSingleFracBits[COMPONENT_Y] + currCompFracBits,
                                                    currCompDistY + currCompDistCr + currCompDistCb, false);
            }
            else
            {
              currCompDistCb =
                m_pcRdCost->getDistPart(csFull->getOrgResiBuf(cbArea), cbResi, channelBitDepth, COMPONENT_Cb, DF_SSE);
              currCompDistCr =
                m_pcRdCost->getDistPart(csFull->getOrgResiBuf(crArea), crResi, channelBitDepth, COMPONENT_Cr, DF_SSE);
#if WCG_EXT
              currCompCost = m_pcRdCost->calcRdCost(currCompFracBits, currCompDistCr + currCompDistCb, false);
#else
              currCompCost = m_pcRdCost->calcRdCost(currCompFracBits, currCompDistCr + currCompDistCb);
#endif
            }
          }
          else
          {
            currCompCost = MAX_DOUBLE;
          }

          // evaluate
          if (currCompCost < minCostCbCr)
          {
            uiSingleDistComp[COMPONENT_Cb] = currCompDistCb;
            uiSingleDistComp[COMPONENT_Cr] = currCompDistCr;
            if (colorTransFlag)
            {
              uiSingleDistComp[COMPONENT_Y] = currCompDistY;
            }
            minCostCbCr = currCompCost;
            {
              bestTU.copyComponentFrom(tu, COMPONENT_Cb);
              bestTU.copyComponentFrom(tu, COMPONENT_Cr);
              saveCS.getResiBuf(cbArea).copyFrom(csFull->getResiBuf(cbArea));
              saveCS.getResiBuf(crArea).copyFrom(csFull->getResiBuf(crArea));
            }
          }

          if (colorTransFlag && (m_pcEncCfg->getCostMode() != COST_LOSSLESS_CODING || !slice.isLossless()))
          {
            m_pcTrQuant->lambdaAdjustColorTrans(false);
          }
        }
      }
      // copy component
      tu.copyComponentFrom(bestTU, COMPONENT_Cb);
      tu.copyComponentFrom(bestTU, COMPONENT_Cr);
      csFull->getResiBuf(cbArea).copyFrom(saveCS.getResiBuf(cbArea));
      csFull->getResiBuf(crArea).copyFrom(saveCS.getResiBuf(crArea));
    }

    m_CABACEstimator->getCtx() = ctxStart;
    m_CABACEstimator->resetBits();
    if( !tu.noResidual )
    {
      static const ComponentID cbf_getComp[MAX_NUM_COMPONENT] = { COMPONENT_Cb, COMPONENT_Cr, COMPONENT_Y };
      for( unsigned c = isChromaEnabled(tu.chromaFormat)?0 : 2; c < MAX_NUM_COMPONENT; c++)
      {
        const ComponentID compID = cbf_getComp[c];
        if (compID == COMPONENT_Y && !luma)
        {
          continue;
        }
        if (compID != COMPONENT_Y && !chroma)
        {
          continue;
        }
        if (tu.blocks[compID].valid())
        {
          const bool prevCbf = (compID == COMPONENT_Cr ? TU::getCbfAtDepth(tu, COMPONENT_Cb, currDepth) : false);
          m_CABACEstimator->cbf_comp(*csFull, TU::getCbfAtDepth(tu, compID, currDepth), tu.blocks[compID], currDepth,
                                     prevCbf);
        }
      }
    }

    for (uint32_t ch = 0; ch < numValidComp; ch++)
    {
      const ComponentID compID = ComponentID(ch);
      if (compID == COMPONENT_Y && !luma)
      {
        continue;
      }
      if (compID != COMPONENT_Y && !chroma)
      {
        continue;
      }
      if (tu.blocks[compID].valid())
      {
        if( compID == COMPONENT_Cr )
        {
          const int cbfMask = ( TU::getCbf( tu, COMPONENT_Cb ) ? 2 : 0 ) + ( TU::getCbf( tu, COMPONENT_Cr ) ? 1 : 0 );
          m_CABACEstimator->joint_cb_cr(tu, cbfMask);
        }
        if( TU::getCbf( tu, compID ) )
        {
          m_CABACEstimator->residual_coding( tu, compID );
        }
        uiSingleDist += uiSingleDistComp[compID];
      }
    }
    if( tu.noResidual )
    {
      CHECK( m_CABACEstimator->getEstFracBits() > 0, "no residual TU's bits shall be 0" );
    }
    if (colorTransFlag)
    {
      PelUnitBuf resiBuf = csFull->getResiBuf(currArea);
      resiBuf.colorSpaceConvert(resiBuf, false, slice.clpRng(COMPONENT_Y));
      if (slice.getLmcsEnabledFlag() && slice.getPicHeader()->getLmcsChromaResidualScaleFlag() && tu.blocks[COMPONENT_Cb].width*tu.blocks[COMPONENT_Cb].height > 4)
      {
        resiBuf.bufs[1].scaleSignal(tu.getChromaAdj(), 0, tu.cu->cs->slice->clpRng(COMPONENT_Cb));
        resiBuf.bufs[2].scaleSignal(tu.getChromaAdj(), 0, tu.cu->cs->slice->clpRng(COMPONENT_Cr));
      }
    }

    csFull->fracBits += m_CABACEstimator->getEstFracBits();
    csFull->dist     += uiSingleDist;
#if WCG_EXT
    if( m_pcEncCfg->getLumaLevelToDeltaQPMapping().isEnabled() )
    {
      csFull->cost    = m_pcRdCost->calcRdCost(csFull->fracBits, csFull->dist, false);
    }
    else
#endif
    {
      csFull->cost = m_pcRdCost->calcRdCost(csFull->fracBits, csFull->dist);
    }
  } // check full

  // code sub-blocks
  if( bCheckSplit )
  {
    if( bCheckFull )
    {
      m_CABACEstimator->getCtx() = ctxStart;
    }

    if( partitioner.canSplit( TU_MAX_TR_SPLIT, cs ) )
    {
      partitioner.splitCurrArea( TU_MAX_TR_SPLIT, cs );
    }
    else if( cu.sbtInfo && partitioner.canSplit( PartSplit( cu.getSbtTuSplit() ), cs ) )
    {
      partitioner.splitCurrArea( PartSplit( cu.getSbtTuSplit() ), cs );
    }
    else
    {
      THROW( "Implicit TU split not available!" );
    }

    do
    {
      xEstimateInterResidualQT(*csSplit, partitioner, bCheckFull ? nullptr : puiZeroDist, luma, chroma, orgResi);

      csSplit->cost = m_pcRdCost->calcRdCost( csSplit->fracBits, csSplit->dist );
    } while( partitioner.nextPart( *csSplit ) );

    partitioner.exitCurrSplit();

    unsigned        anyCbfSet   =   0;
    unsigned        compCbf[3]  = { 0, 0, 0 };

    if( !bCheckFull )
    {
      for( auto &currTU : csSplit->traverseTUs( currArea, partitioner.chType ) )
      {
        for( unsigned ch = 0; ch < numTBlocks; ch++ )
        {
          compCbf[ ch ] |= ( TU::getCbfAtDepth( currTU, ComponentID(ch), currDepth + 1 ) ? 1 : 0 );
        }
      }

      for (auto &currTU: csSplit->traverseTUs(currArea, partitioner.chType))
      {
        TU::setCbfAtDepth(currTU, COMPONENT_Y, currDepth, compCbf[COMPONENT_Y]);
        if( currArea.chromaFormat != CHROMA_400 )
        {
          TU::setCbfAtDepth(currTU, COMPONENT_Cb, currDepth, compCbf[COMPONENT_Cb]);
          TU::setCbfAtDepth(currTU, COMPONENT_Cr, currDepth, compCbf[COMPONENT_Cr]);
        }
      }

      anyCbfSet = compCbf[COMPONENT_Y];
      if (currArea.chromaFormat != CHROMA_400)
      {
        anyCbfSet |= compCbf[COMPONENT_Cb];
        anyCbfSet |= compCbf[COMPONENT_Cr];
      }

      m_CABACEstimator->getCtx() = ctxStart;
      m_CABACEstimator->resetBits();

      // when compID isn't a channel, code Cbfs:
      xEncodeInterResidualQT( *csSplit, partitioner, MAX_NUM_TBLOCKS );
      for (uint32_t ch = 0; ch < numValidComp; ch++)
      {
        const ComponentID compID = ComponentID(ch);
        if (compID == COMPONENT_Y && !luma)
          continue;
        if (compID != COMPONENT_Y && !chroma)
          continue;
        xEncodeInterResidualQT( *csSplit, partitioner, ComponentID( ch ) );
      }

      csSplit->fracBits = m_CABACEstimator->getEstFracBits();
      csSplit->cost     = m_pcRdCost->calcRdCost(csSplit->fracBits, csSplit->dist);

      if( bCheckFull && anyCbfSet && csSplit->cost < csFull->cost )
      {
        cs.useSubStructure( *csSplit, partitioner.chType, currArea, false, false, false, true, true );
        cs.cost = csSplit->cost;
      }
    }


    if( csSplit && csFull )
    {
      csSplit->releaseIntermediateData();
      csFull ->releaseIntermediateData();
    }
  }
}

void InterSearch::encodeResAndCalcRdInterCU(CodingStructure &cs, Partitioner &partitioner, const bool &skipResidual
  , const bool luma, const bool chroma
)
{
  m_pcRdCost->setChromaFormat(cs.sps->getChromaFormatIdc());

  CodingUnit &cu = *cs.getCU( partitioner.chType );
  if( cu.predMode == MODE_INTER )
    CHECK( cu.isSepTree(), "CU with Inter mode must be in single tree" );

  const ChromaFormat format     = cs.area.chromaFormat;;
  const int  numValidComponents = getNumberValidComponents(format);
  const SPS &sps                = *cs.sps;

  bool colorTransAllowed = cs.slice->getSPS()->getUseColorTrans() && luma && chroma;
  if (cs.slice->getSPS()->getUseColorTrans())
  {
    CHECK(cu.treeType != TREE_D || partitioner.treeType != TREE_D, "localtree should not be applied when adaptive color transform is enabled");
    CHECK(cu.modeType != MODE_TYPE_ALL || partitioner.modeType != MODE_TYPE_ALL, "localtree should not be applied when adaptive color transform is enabled");
  }

  if( skipResidual ) //  No residual coding : SKIP mode
  {
    cu.skip    = true;
    cu.rootCbf = false;
    cu.colorTransform = false;
    CHECK( cu.sbtInfo != 0, "sbtInfo shall be 0 if CU has no residual" );
    cs.getResiBuf().fill(0);
    {
      cs.getRecoBuf().copyFrom(cs.getPredBuf() );
      if (m_pcEncCfg->getLmcs() && (cs.slice->getLmcsEnabledFlag() && m_pcReshape->getCTUFlag()) && !cu.firstPU->ciipFlag && !CU::isIBC(cu))
      {
        cs.getRecoBuf().Y().rspSignal(m_pcReshape->getFwdLUT());
      }
    }


    // add empty TU(s)
    cs.addEmptyTUs( partitioner );
    Distortion distortion = 0;

    for (int comp = 0; comp < numValidComponents; comp++)
    {
      const ComponentID compID = ComponentID(comp);
      if (compID == COMPONENT_Y && !luma)
      {
        continue;
      }
      if (compID != COMPONENT_Y && !chroma)
      {
        continue;
      }
      CPelBuf reco = cs.getRecoBuf (compID);
      CPelBuf org  = cs.getOrgBuf  (compID);
#if WCG_EXT
      if (m_pcEncCfg->getLumaLevelToDeltaQPMapping().isEnabled() || (
        m_pcEncCfg->getLmcs() && (cs.slice->getLmcsEnabledFlag() && m_pcReshape->getCTUFlag())))
      {
        const CPelBuf orgLuma = cs.getOrgBuf( cs.area.blocks[COMPONENT_Y] );
        if (compID == COMPONENT_Y && !(m_pcEncCfg->getLumaLevelToDeltaQPMapping().isEnabled()))
        {
          const CompArea &areaY = cu.Y();
          CompArea      tmpArea1(COMPONENT_Y, areaY.chromaFormat, Position(0, 0), areaY.size());
          PelBuf tmpRecLuma = m_tmpStorageLCU.getBuf(tmpArea1);
          tmpRecLuma.copyFrom(reco);
          tmpRecLuma.rspSignal(m_pcReshape->getInvLUT());
          distortion += m_pcRdCost->getDistPart(org, tmpRecLuma, sps.getBitDepth(toChannelType(compID)), compID, DF_SSE_WTD, &orgLuma);
        }
        else
        {
          distortion +=
            m_pcRdCost->getDistPart(org, reco, sps.getBitDepth(toChannelType(compID)), compID, DF_SSE_WTD, &orgLuma);
        }
      }
      else
#endif
      {
        distortion += m_pcRdCost->getDistPart(org, reco, sps.getBitDepth(toChannelType(compID)), compID, DF_SSE);
      }
    }

    m_CABACEstimator->resetBits();

    PredictionUnit &pu = *cs.getPU( partitioner.chType );

    m_CABACEstimator->cu_skip_flag  ( cu );
    m_CABACEstimator->merge_data(pu);

    cs.dist     = distortion;
    cs.fracBits = m_CABACEstimator->getEstFracBits();
    cs.cost     = m_pcRdCost->calcRdCost(cs.fracBits, cs.dist);

    return;
  }

  //  Residual coding.
  if (luma)
  {
    cs.getResiBuf().bufs[0].copyFrom(cs.getOrgBuf().bufs[0]);
    if (cs.slice->getLmcsEnabledFlag() && m_pcReshape->getCTUFlag())
    {
      const CompArea &areaY = cu.Y();
      CompArea      tmpArea(COMPONENT_Y, areaY.chromaFormat, Position(0, 0), areaY.size());
      PelBuf tmpPred = m_tmpStorageLCU.getBuf(tmpArea);
      tmpPred.copyFrom(cs.getPredBuf(COMPONENT_Y));

      if (!cu.firstPU->ciipFlag && !CU::isIBC(cu))
      {
        tmpPred.rspSignal(m_pcReshape->getFwdLUT());
      }
      cs.getResiBuf(COMPONENT_Y).rspSignal(m_pcReshape->getFwdLUT());
      cs.getResiBuf(COMPONENT_Y).subtract(tmpPred);
    }
    else
    {
      cs.getResiBuf().bufs[0].subtract(cs.getPredBuf().bufs[0]);
    }
  }
  if (chroma && isChromaEnabled(cs.pcv->chrFormat))
  {
    cs.getResiBuf().bufs[1].copyFrom(cs.getOrgBuf().bufs[1]);
    cs.getResiBuf().bufs[2].copyFrom(cs.getOrgBuf().bufs[2]);
    cs.getResiBuf().bufs[1].subtract(cs.getPredBuf().bufs[1]);
    cs.getResiBuf().bufs[2].subtract(cs.getPredBuf().bufs[2]);
  }
  const UnitArea curUnitArea = partitioner.currArea();
  CodingStructure &saveCS = *m_pSaveCS[1];
  saveCS.pcv = cs.pcv;
  saveCS.picture = cs.picture;
  saveCS.area.repositionTo(curUnitArea);
  saveCS.clearCUs();
  saveCS.clearPUs();
  saveCS.clearTUs();
  for (const auto &ppcu : cs.cus)
  {
    CodingUnit &pcu = saveCS.addCU(*ppcu, ppcu->chType);
    pcu = *ppcu;
  }
  for (const auto &ppu : cs.pus)
  {
    PredictionUnit &pu = saveCS.addPU(*ppu, ppu->chType);
    pu = *ppu;
  }

  PelUnitBuf orgResidual;
  const UnitArea localUnitArea(cs.area.chromaFormat, Area(0, 0, cu.Y().width, cu.Y().height));
  orgResidual = m_colorTransResiBuf[0].getBuf(localUnitArea);
  orgResidual.copyFrom(cs.getResiBuf());

  const TempCtx ctxStart(m_CtxCache, m_CABACEstimator->getCtx());
  int           numAllowedColorSpace = (colorTransAllowed ? 2 : 1);
  Distortion    zeroDistortion = 0;

  double  bestCost = MAX_DOUBLE;
  bool    bestColorTrans = false;
  bool    bestRootCbf = false;
  uint8_t bestsbtInfo = 0;
  uint8_t orgSbtInfo = cu.sbtInfo;
  int     bestIter = 0;

  auto blkCache = dynamic_cast<CacheBlkInfoCtrl*>(m_modeCtrl);
  bool rootCbfFirstColorSpace = true;

  for (int iter = 0; iter < numAllowedColorSpace; iter++)
  {
    if (colorTransAllowed && !m_pcEncCfg->getRGBFormatFlag() && iter)
    {
      continue;
    }
    char colorSpaceOption = blkCache->getSelectColorSpaceOption(cu);
    if (colorTransAllowed)
    {
      if (colorSpaceOption)
      {
        CHECK(colorSpaceOption > 2 || colorSpaceOption < 0, "invalid color space selection option");
        if (colorSpaceOption == 1 && iter)
        {
          continue;
        }
        if (colorSpaceOption == 2 && !iter)
        {
          continue;
        }
      }
    }
    if (!colorSpaceOption)
    {
      if (iter && !rootCbfFirstColorSpace)
      {
        continue;
      }
      if (colorTransAllowed && cs.bestParent && cs.bestParent->tmpColorSpaceCost != MAX_DOUBLE)
      {
        if (cs.bestParent->firstColorSpaceSelected && iter)
        {
          continue;
        }
        if (m_pcEncCfg->getRGBFormatFlag())
        {
          if (!cs.bestParent->firstColorSpaceSelected && !iter)
          {
            continue;
          }
        }
      }
    }
    bool colorTransFlag = (colorTransAllowed && m_pcEncCfg->getRGBFormatFlag()) ? (1 - iter) : iter;
    cu.colorTransform = colorTransFlag;
    cu.sbtInfo = orgSbtInfo;

    m_CABACEstimator->resetBits();
    m_CABACEstimator->getCtx() = ctxStart;
    cs.clearTUs();
    cs.fracBits = 0;
    cs.dist = 0;
    cs.cost = 0;

    if (colorTransFlag)
    {
      cs.getOrgResiBuf().bufs[0].copyFrom(orgResidual.bufs[0]);
      cs.getOrgResiBuf().bufs[1].copyFrom(orgResidual.bufs[1]);
      cs.getOrgResiBuf().bufs[2].copyFrom(orgResidual.bufs[2]);

      memset(m_pTempPel, 0, sizeof(Pel) * localUnitArea.blocks[0].area());
      zeroDistortion = 0;
      for (int compIdx = 0; compIdx < 3; compIdx++)
      {
        ComponentID   componentID = (ComponentID) compIdx;
        const CPelBuf zeroBuf(m_pTempPel, localUnitArea.blocks[compIdx]);
        zeroDistortion += m_pcRdCost->getDistPart(zeroBuf, orgResidual.bufs[compIdx],
                                                  sps.getBitDepth(toChannelType(componentID)), componentID, DF_SSE);
      }
      xEstimateInterResidualQT(cs, partitioner, nullptr, luma, chroma, &orgResidual);
    }
    else
    {
      zeroDistortion = 0;
      if (luma)
      {
        cs.getOrgResiBuf().bufs[0].copyFrom(orgResidual.bufs[0]);
      }
      if (chroma && isChromaEnabled(cs.pcv->chrFormat))
      {
        cs.getOrgResiBuf().bufs[1].copyFrom(orgResidual.bufs[1]);
        cs.getOrgResiBuf().bufs[2].copyFrom(orgResidual.bufs[2]);
      }
      xEstimateInterResidualQT(cs, partitioner, &zeroDistortion, luma, chroma);
    }
    TransformUnit &firstTU = *cs.getTU(partitioner.chType);

    cu.rootCbf = false;
    m_CABACEstimator->resetBits();
    m_CABACEstimator->rqt_root_cbf(cu);
    const uint64_t zeroFracBits = m_CABACEstimator->getEstFracBits();
    double         zeroCost;
    {
#if WCG_EXT
      if (m_pcEncCfg->getLumaLevelToDeltaQPMapping().isEnabled())
      {
        zeroCost = m_pcRdCost->calcRdCost(zeroFracBits, zeroDistortion, false);
      }
      else
#endif
      {
        zeroCost = m_pcRdCost->calcRdCost(zeroFracBits, zeroDistortion);
      }
    }

    const int numValidTBlocks = ::getNumberValidTBlocks(*cs.pcv);
    for (uint32_t i = 0; i < numValidTBlocks; i++)
    {
      cu.rootCbf |= TU::getCbfAtDepth(firstTU, ComponentID(i), 0);
    }

    // -------------------------------------------------------
    // If a block full of 0's is efficient, then just use 0's.
    // The costs at this point do not include header bits.

    if (zeroCost < cs.cost || !cu.rootCbf)
    {
      cs.cost           = zeroCost;
      cu.colorTransform = false;
      cu.sbtInfo        = 0;
      cu.rootCbf        = false;

      cs.clearTUs();

      // add new "empty" TU(s) spanning the whole CU
      cs.addEmptyTUs(partitioner);
    }
    if (!iter)
    {
      rootCbfFirstColorSpace = cu.rootCbf;
    }
    if (cs.cost < bestCost)
    {
      bestIter = iter;

      if (iter != (numAllowedColorSpace - 1))
      {
        bestCost       = cs.cost;
        bestColorTrans = cu.colorTransform;
        bestRootCbf    = cu.rootCbf;
        bestsbtInfo    = cu.sbtInfo;

        saveCS.clearTUs();
        for (const auto &ptu: cs.tus)
        {
          TransformUnit &tu = saveCS.addTU(*ptu, ptu->chType);
          tu                = *ptu;
        }
        saveCS.getResiBuf(curUnitArea).copyFrom(cs.getResiBuf(curUnitArea));
      }
    }
  }

  if (bestIter != (numAllowedColorSpace - 1))
  {
    cu.colorTransform = bestColorTrans;
    cu.rootCbf = bestRootCbf;
    cu.sbtInfo = bestsbtInfo;

    cs.clearTUs();
    for (const auto &ptu : saveCS.tus)
    {
      TransformUnit &tu = cs.addTU(*ptu, ptu->chType);
      tu = *ptu;
    }
    cs.getResiBuf(curUnitArea).copyFrom(saveCS.getResiBuf(curUnitArea));
  }

  // all decisions now made. Fully encode the CU, including the headers:
  m_CABACEstimator->getCtx() = ctxStart;

  uint64_t finalFracBits = xGetSymbolFracBitsInter( cs, partitioner );
  // we've now encoded the CU, and so have a valid bit cost
  if (!cu.rootCbf)
  {
    if (luma)
    {
      cs.getResiBuf().bufs[0].fill(0); // Clear the residual image, if we didn't code it.
    }
    if (chroma && isChromaEnabled(cs.pcv->chrFormat))
    {
      cs.getResiBuf().bufs[1].fill(0); // Clear the residual image, if we didn't code it.
      cs.getResiBuf().bufs[2].fill(0); // Clear the residual image, if we didn't code it.
    }
  }

  if (luma)
  {
    if (cu.rootCbf && cs.slice->getLmcsEnabledFlag() && m_pcReshape->getCTUFlag())
    {
      const CompArea &areaY = cu.Y();
      CompArea      tmpArea(COMPONENT_Y, areaY.chromaFormat, Position(0, 0), areaY.size());
      PelBuf tmpPred = m_tmpStorageLCU.getBuf(tmpArea);
      tmpPred.copyFrom(cs.getPredBuf(COMPONENT_Y));

      if (!cu.firstPU->ciipFlag && !CU::isIBC(cu))
      {
        tmpPred.rspSignal(m_pcReshape->getFwdLUT());
      }

      cs.getRecoBuf(COMPONENT_Y).reconstruct(tmpPred, cs.getResiBuf(COMPONENT_Y), cs.slice->clpRng(COMPONENT_Y));
    }
    else
    {
      cs.getRecoBuf().bufs[0].reconstruct(cs.getPredBuf().bufs[0], cs.getResiBuf().bufs[0], cs.slice->clpRngs().comp[0]);
      if (cs.slice->getLmcsEnabledFlag() && m_pcReshape->getCTUFlag() && !cu.firstPU->ciipFlag && !CU::isIBC(cu))
      {
        cs.getRecoBuf().bufs[0].rspSignal(m_pcReshape->getFwdLUT());
      }
    }
  }
  if (chroma && isChromaEnabled(cs.pcv->chrFormat))
  {
    cs.getRecoBuf().bufs[1].reconstruct(cs.getPredBuf().bufs[1], cs.getResiBuf().bufs[1], cs.slice->clpRngs().comp[1]);
    cs.getRecoBuf().bufs[2].reconstruct(cs.getPredBuf().bufs[2], cs.getResiBuf().bufs[2], cs.slice->clpRngs().comp[2]);
  }

  // update with clipped distortion and cost (previously unclipped reconstruction values were used)
  Distortion finalDistortion = 0;

  for (int comp = 0; comp < numValidComponents; comp++)
  {
    const ComponentID compID = ComponentID(comp);
    if (compID == COMPONENT_Y && !luma)
    {
      continue;
    }
    if (compID != COMPONENT_Y && !chroma)
    {
      continue;
    }
    CPelBuf reco = cs.getRecoBuf (compID);
    CPelBuf org  = cs.getOrgBuf  (compID);

#if WCG_EXT
    if (m_pcEncCfg->getLumaLevelToDeltaQPMapping().isEnabled() || (
      m_pcEncCfg->getLmcs() && (cs.slice->getLmcsEnabledFlag() && m_pcReshape->getCTUFlag())))
    {
      const CPelBuf orgLuma = cs.getOrgBuf( cs.area.blocks[COMPONENT_Y] );
      if (compID == COMPONENT_Y && !(m_pcEncCfg->getLumaLevelToDeltaQPMapping().isEnabled()) )
      {
        const CompArea &areaY = cu.Y();
        CompArea      tmpArea1(COMPONENT_Y, areaY.chromaFormat, Position(0, 0), areaY.size());
        PelBuf tmpRecLuma = m_tmpStorageLCU.getBuf(tmpArea1);
        tmpRecLuma.copyFrom(reco);
        tmpRecLuma.rspSignal(m_pcReshape->getInvLUT());
        finalDistortion += m_pcRdCost->getDistPart(org, tmpRecLuma, sps.getBitDepth(toChannelType(compID)), compID, DF_SSE_WTD, &orgLuma);
      }
      else
      {
        finalDistortion += m_pcRdCost->getDistPart(org, reco, sps.getBitDepth(toChannelType(compID)), compID, DF_SSE_WTD, &orgLuma);
      }
    }
    else
#endif
    {
      finalDistortion += m_pcRdCost->getDistPart( org, reco, sps.getBitDepth( toChannelType( compID ) ), compID, DF_SSE );
    }
  }

  cs.dist     = finalDistortion;
  cs.fracBits = finalFracBits;
  cs.cost     = m_pcRdCost->calcRdCost(cs.fracBits, cs.dist);
  if (cs.slice->getSPS()->getUseColorTrans())
  {
    if (cs.cost < cs.tmpColorSpaceCost)
    {
      cs.tmpColorSpaceCost = cs.cost;
      if (m_pcEncCfg->getRGBFormatFlag())
      {
        cs.firstColorSpaceSelected = cu.colorTransform || !cu.rootCbf;
      }
      else
      {
        cs.firstColorSpaceSelected = !cu.colorTransform || !cu.rootCbf;
      }
    }
  }

  CHECK(cs.tus.size() == 0, "No TUs present");
}

uint64_t InterSearch::xGetSymbolFracBitsInter(CodingStructure &cs, Partitioner &partitioner)
{
  uint64_t fracBits   = 0;
  CodingUnit &cu    = *cs.getCU( partitioner.chType );

  m_CABACEstimator->resetBits();

  if( cu.firstPU->mergeFlag && !cu.rootCbf )
  {
    cu.skip = true;
    CHECK(cu.colorTransform, "ACT should not be enabled for skip mode");
    m_CABACEstimator->cu_skip_flag  ( cu );
    if (cu.firstPU->ciipFlag)
    {
      // CIIP shouldn't be skip, the upper level function will deal with it, i.e. setting the overall cost to MAX_DOUBLE
    }
    else
    {
      m_CABACEstimator->merge_data(*cu.firstPU);
    }
    fracBits   += m_CABACEstimator->getEstFracBits();
  }
  else
  {
    CHECK( cu.skip, "Skip flag has to be off at this point!" );

    if (cu.Y().valid())
    m_CABACEstimator->cu_skip_flag( cu );
    m_CABACEstimator->pred_mode   ( cu );
    m_CABACEstimator->cu_pred_data( cu );
    CUCtx cuCtx;
    cuCtx.isDQPCoded = true;
    cuCtx.isChromaQpAdjCoded = true;
    m_CABACEstimator->cu_residual ( cu, partitioner, cuCtx );
    fracBits       += m_CABACEstimator->getEstFracBits();
  }

  return fracBits;
}

double InterSearch::xGetMEDistortionWeight(uint8_t bcwIdx, RefPicList eRefPicList)
{
  if( bcwIdx != BCW_DEFAULT )
  {
    return fabs((double)getBcwWeight(bcwIdx, eRefPicList) / (double)g_BcwWeightBase);
  }
  else
  {
    return 0.5;
  }
}
#if GDR_ENABLED
bool InterSearch::xReadBufferedUniMv(PredictionUnit &pu, RefPicList eRefPicList, int32_t refIdx, Mv &pcMvPred, Mv &rcMv,
                                     bool &rcMvSolid, uint32_t &ruiBits, Distortion &ruiCost)
#else
bool InterSearch::xReadBufferedUniMv(PredictionUnit &pu, RefPicList eRefPicList, int32_t refIdx, Mv &pcMvPred, Mv &rcMv,
                                     uint32_t &ruiBits, Distortion &ruiCost)
#endif
{
  if (m_uniMotions.isReadMode((uint32_t) eRefPicList, (uint32_t) refIdx))
  {
#if GDR_ENABLED
    m_uniMotions.copyTo(rcMv, rcMvSolid, ruiCost, (uint32_t) eRefPicList, (uint32_t) refIdx);
#else
    m_uniMotions.copyTo(rcMv, ruiCost, (uint32_t) eRefPicList, (uint32_t) refIdx);
#endif

    Mv pred = pcMvPred;
    pred.changeTransPrecInternal2Amvr(pu.cu->imv);
    m_pcRdCost->setPredictor(pred);
    m_pcRdCost->setCostScale(0);

    Mv mv = rcMv;
    mv.changeTransPrecInternal2Amvr(pu.cu->imv);
    uint32_t mvBits = m_pcRdCost->getBitsOfVectorWithPredictor(mv.getHor(), mv.getVer(), 0);

    ruiBits += mvBits;
    ruiCost += m_pcRdCost->getCost(ruiBits);
    return true;
  }
  return false;
}

#if GDR_ENABLED
bool InterSearch::xReadBufferedAffineUniMv(PredictionUnit &pu, RefPicList eRefPicList, int32_t refIdx, Mv acMvPred[3],
                                           Mv acMv[3], bool acMvSolid[3], uint32_t &ruiBits, Distortion &ruiCost,
                                           int &mvpIdx, const AffineAMVPInfo &aamvpi)
#else
bool InterSearch::xReadBufferedAffineUniMv(PredictionUnit &pu, RefPicList eRefPicList, int32_t refIdx, Mv acMvPred[3],
                                           Mv acMv[3], uint32_t &ruiBits, Distortion &ruiCost, int &mvpIdx,
                                           const AffineAMVPInfo &aamvpi)
#endif
{
  if (m_uniMotions.isReadModeAffine((uint32_t) eRefPicList, (uint32_t) refIdx, pu.cu->affineType))
  {
#if GDR_ENABLED
    m_uniMotions.copyAffineMvTo(acMv, acMvSolid, ruiCost, (uint32_t) eRefPicList, (uint32_t) refIdx, pu.cu->affineType,
                                mvpIdx);
#else
    m_uniMotions.copyAffineMvTo(acMv, ruiCost, (uint32_t) eRefPicList, (uint32_t) refIdx, pu.cu->affineType, mvpIdx);
#endif
    m_pcRdCost->setCostScale(0);
    acMvPred[0] = aamvpi.mvCandLT[mvpIdx];
    acMvPred[1] = aamvpi.mvCandRT[mvpIdx];
    acMvPred[2] = aamvpi.mvCandLB[mvpIdx];

    uint32_t mvBits = 0;
    for (int verIdx = 0; verIdx<(pu.cu->affineType ? 3 : 2); verIdx++)
    {
      Mv pred = verIdx ? acMvPred[verIdx] + acMv[0] - acMvPred[0] : acMvPred[verIdx];
      pred.changePrecision(MV_PRECISION_INTERNAL, MV_PRECISION_QUARTER);
      m_pcRdCost->setPredictor(pred);
      Mv mv = acMv[verIdx];
      mv.changePrecision(MV_PRECISION_INTERNAL, MV_PRECISION_QUARTER);
      mvBits += m_pcRdCost->getBitsOfVectorWithPredictor(mv.getHor(), mv.getVer(), 0);
    }
    ruiBits += mvBits;
    ruiCost += m_pcRdCost->getCost(ruiBits);
    return true;
  }
  return false;
}

void InterSearch::initWeightIdxBits()
{
  for (int n = 0; n < BCW_NUM; ++n)
  {
    m_estWeightIdxBits[n] = deriveWeightIdxBits(n);
  }
}

void InterSearch::xClipMv( Mv& rcMv, const Position& pos, const struct Size& size, const SPS& sps, const PPS& pps )
{
  int mvShift = MV_FRACTIONAL_BITS_INTERNAL;
  int offset = 8;

  int horMax = ( pps.getPicWidthInLumaSamples() + offset - (int)pos.x - 1 ) << mvShift;
  int horMin = (-(int) sps.getMaxCUWidth() - offset - (int) pos.x + 1) * (1 << mvShift);

  int verMax = ( pps.getPicHeightInLumaSamples() + offset - (int)pos.y - 1 ) << mvShift;
  int verMin = (-(int) sps.getMaxCUHeight() - offset - (int) pos.y + 1) * (1 << mvShift);

  const SubPic &curSubPic = pps.getSubPicFromPos(pos);
  if (curSubPic.getTreatedAsPicFlag() && m_clipMvInSubPic)
  {
    horMax = ((curSubPic.getSubPicRight() + 1)  + offset - (int)pos.x - 1) << mvShift;
    horMin = (-(int) sps.getMaxCUWidth() - offset - ((int) pos.x - curSubPic.getSubPicLeft()) + 1) * (1 << mvShift);

    verMax = ((curSubPic.getSubPicBottom() + 1) + offset -  (int)pos.y - 1) << mvShift;
    verMin = (-(int) sps.getMaxCUHeight() - offset - ((int) pos.y - curSubPic.getSubPicTop()) + 1) * (1 << mvShift);
  }
  if( pps.getWrapAroundEnabledFlag() )
  {
    int horMax = ( pps.getPicWidthInLumaSamples() + sps.getMaxCUWidth() - size.width + offset - (int)pos.x - 1 ) << mvShift;
    int horMin = (-(int) sps.getMaxCUWidth() - offset - (int) pos.x + 1) * (1 << mvShift);
    rcMv.setHor( std::min( horMax, std::max( horMin, rcMv.getHor() ) ) );
    rcMv.setVer( std::min( verMax, std::max( verMin, rcMv.getVer() ) ) );
    return;
  }

  rcMv.setHor( std::min( horMax, std::max( horMin, rcMv.getHor() ) ) );
  rcMv.setVer( std::min( verMax, std::max( verMin, rcMv.getVer() ) ) );
}

uint32_t InterSearch::xDetermineBestMvp( PredictionUnit& pu, Mv acMvTemp[3], int& mvpIdx, const AffineAMVPInfo& aamvpi )
{
  bool mvpUpdated  = false;
  uint32_t minBits = std::numeric_limits<uint32_t>::max();
#if GDR_ENABLED
  const CodingStructure &cs = *pu.cs;
  const bool isEncodeGdrClean = cs.sps->getGDREnabledFlag() && cs.pcv->isEncoder && ((cs.picHeader->getInGdrInterval() && cs.isClean(pu.Y().topRight(), CHANNEL_TYPE_LUMA)) || (cs.picHeader->getNumVerVirtualBoundaries() == 0));
#endif

  for ( int i = 0; i < aamvpi.numCand; i++ )
  {
    Mv mvPred[3] = { aamvpi.mvCandLT[i], aamvpi.mvCandRT[i], aamvpi.mvCandLB[i] };
    uint32_t candBits = m_auiMVPIdxCost[i][aamvpi.numCand];
    candBits += xCalcAffineMVBits( pu, acMvTemp, mvPred );

#if GDR_ENABLED
    bool isSolid = true;
    if (isEncodeGdrClean)
    {
      isSolid = aamvpi.mvSolidLT[i] && aamvpi.mvSolidRT[i];
      if (pu.cu->affineType == AFFINEMODEL_6PARAM)
      {
        isSolid = isSolid && aamvpi.mvSolidLB[i];
      }
    }

    if ((candBits < minBits) && isSolid)
#else
    if ( candBits < minBits )
#endif
    {
      minBits    = candBits;
      mvpIdx     = i;
      mvpUpdated = true;
    }
  }

#if GDR_ENABLED
  mvpUpdated = true; // do not check mvp update for GDR
#endif

  CHECK( !mvpUpdated, "xDetermineBestMvp() error" );

  return minBits;
}

void InterSearch::symmvdCheckBestMvp(
  PredictionUnit& pu,
  PelUnitBuf& origBuf,
  Mv curMv,
  RefPicList curRefList,
  AMVPInfo amvpInfo[2][33],
  int32_t bcwIdx,
  Mv cMvPredSym[2],
#if GDR_ENABLED
  bool cMvPredSymSolid[2],
#endif
  int32_t mvpIdxSym[2],
  Distortion& bestCost,
  bool skip
)
{
#if GDR_ENABLED
  CodingStructure &cs = *pu.cs;
  const bool isEncodeGdrClean = cs.sps->getGDREnabledFlag() && cs.pcv->isEncoder && ((cs.picHeader->getInGdrInterval() && cs.isClean(pu.Y().topRight(), CHANNEL_TYPE_LUMA)) || (cs.picHeader->getNumVerVirtualBoundaries() == 0));
  bool bestCostOk = true;
  bool costOk = true;
  bool allOk;
#endif

  RefPicList tarRefList = (RefPicList)(1 - curRefList);
  int32_t refIdxCur = pu.cu->slice->getSymRefIdx(curRefList);
  int32_t refIdxTar = pu.cu->slice->getSymRefIdx(tarRefList);

  MvField cCurMvField, cTarMvField;
  cCurMvField.setMvField(curMv, refIdxCur);
  AMVPInfo& amvpCur = amvpInfo[curRefList][refIdxCur];
  AMVPInfo& amvpTar = amvpInfo[tarRefList][refIdxTar];
  m_pcRdCost->setCostScale(0);


  // get prediction of eCurRefPicList
  PelUnitBuf predBufA = m_tmpPredStorage[curRefList].getBuf(UnitAreaRelative(*pu.cu, pu));
  const Picture* picRefA = pu.cu->slice->getRefPic(curRefList, cCurMvField.refIdx);
  Mv mvA = cCurMvField.mv;
  clipMv( mvA, pu.cu->lumaPos(), pu.cu->lumaSize(), *pu.cs->sps, *pu.cs->pps );
  if ( (mvA.hor & 15) == 0 && (mvA.ver & 15) == 0 )
  {
    Position offset = pu.blocks[COMPONENT_Y].pos().offset( mvA.getHor() >> 4, mvA.getVer() >> 4 );
    CPelBuf pelBufA = picRefA->getRecoBuf( CompArea( COMPONENT_Y, pu.chromaFormat, offset, pu.blocks[COMPONENT_Y].size() ), false );
    predBufA.bufs[0].buf = const_cast<Pel *>(pelBufA.buf);
    predBufA.bufs[0].stride = pelBufA.stride;
  }
  else
  {
    xPredInterBlk( COMPONENT_Y, pu, picRefA, mvA, predBufA, false, pu.cu->slice->clpRng( COMPONENT_Y ), false, false );
  }
  PelUnitBuf bufTmp = m_tmpStorageLCU.getBuf( UnitAreaRelative( *pu.cu, pu ) );
  bufTmp.copyFrom( origBuf );
  bufTmp.removeHighFreq( predBufA, m_pcEncCfg->getClipForBiPredMeEnabled(), pu.cu->slice->clpRngs(), getBcwWeight( pu.cu->BcwIdx, tarRefList ) );

  double fWeight = xGetMEDistortionWeight( pu.cu->BcwIdx, tarRefList );

  int32_t skipMvpIdx[2];
  skipMvpIdx[0] = skip ? mvpIdxSym[0] : -1;
  skipMvpIdx[1] = skip ? mvpIdxSym[1] : -1;

  for (int i = 0; i < amvpCur.numCand; i++)
  {
    for (int j = 0; j < amvpTar.numCand; j++)
    {
      if (skipMvpIdx[curRefList] == i && skipMvpIdx[tarRefList] == j)
      {
        continue;
      }

      cTarMvField.setMvField(curMv.getSymmvdMv(amvpCur.mvCand[i], amvpTar.mvCand[j]), refIdxTar);

      // get prediction of eTarRefPicList
      PelUnitBuf predBufB = m_tmpPredStorage[tarRefList].getBuf(UnitAreaRelative(*pu.cu, pu));
      const Picture* picRefB = pu.cu->slice->getRefPic(tarRefList, cTarMvField.refIdx);
      Mv mvB = cTarMvField.mv;
      clipMv( mvB, pu.cu->lumaPos(), pu.cu->lumaSize(), *pu.cs->sps, *pu.cs->pps );
      if ( (mvB.hor & 15) == 0 && (mvB.ver & 15) == 0 )
      {
        Position offset = pu.blocks[COMPONENT_Y].pos().offset( mvB.getHor() >> 4, mvB.getVer() >> 4 );
        CPelBuf pelBufB = picRefB->getRecoBuf( CompArea( COMPONENT_Y, pu.chromaFormat, offset, pu.blocks[COMPONENT_Y].size() ), false );
        predBufB.bufs[0].buf = const_cast<Pel *>(pelBufB.buf);
        predBufB.bufs[0].stride = pelBufB.stride;
      }
      else
      {
        xPredInterBlk( COMPONENT_Y, pu, picRefB, mvB, predBufB, false, pu.cu->slice->clpRng( COMPONENT_Y ), false, false );
      }
      // calc distortion
      DFunc distFunc = (!pu.cu->slice->getDisableSATDForRD()) ? DF_HAD : DF_SAD;
      Distortion cost = (Distortion)floor( fWeight * (double)m_pcRdCost->getDistPart( bufTmp.Y(), predBufB.Y(), pu.cs->sps->getBitDepth( CHANNEL_TYPE_LUMA ), COMPONENT_Y, distFunc ) );

      Mv pred = amvpCur.mvCand[i];
      pred.changeTransPrecInternal2Amvr(pu.cu->imv);
      m_pcRdCost->setPredictor(pred);
      Mv mv = curMv;
      mv.changeTransPrecInternal2Amvr(pu.cu->imv);
      uint32_t bits = m_pcRdCost->getBitsOfVectorWithPredictor(mv.hor, mv.ver, 0);
      bits += m_auiMVPIdxCost[i][AMVP_MAX_NUM_CANDS];
      bits += m_auiMVPIdxCost[j][AMVP_MAX_NUM_CANDS];
      cost += m_pcRdCost->getCost(bits);
#if GDR_ENABLED
      if (isEncodeGdrClean)
      {
        bool curSolid = amvpCur.mvSolid[i];
        bool tarSolid = amvpTar.mvSolid[j];
        costOk = curSolid && tarSolid;
      }
#endif


#if GDR_ENABLED
      allOk = (cost < bestCost);
      if (isEncodeGdrClean)
      {
        if (costOk)
        {
          allOk = (bestCostOk) ? (cost < bestCost) : true;
        }
        else
        {
          allOk = false;
        }
      }
#endif

#if GDR_ENABLED
      if (allOk)
#else
      if (cost < bestCost)
#endif
      {
        bestCost = cost;
        cMvPredSym[curRefList] = amvpCur.mvCand[i];
        cMvPredSym[tarRefList] = amvpTar.mvCand[j];
#if GDR_ENABLED
        if (isEncodeGdrClean)
        {
          bestCostOk = costOk;
          cMvPredSymSolid[curRefList] = amvpCur.mvSolid[i];
          cMvPredSymSolid[tarRefList] = amvpTar.mvSolid[j];
        }
#endif
        mvpIdxSym[curRefList] = i;
        mvpIdxSym[tarRefList] = j;
      }
    }
  }
}

uint64_t InterSearch::xCalcPuMeBits(PredictionUnit& pu)
{
  assert(pu.mergeFlag);
  assert(!CU::isIBC(*pu.cu));
  m_CABACEstimator->resetBits();
  m_CABACEstimator->merge_flag(pu);
  if (pu.mergeFlag)
  {
    m_CABACEstimator->merge_data(pu);
  }
  return m_CABACEstimator->getEstFracBits();
}

bool InterSearch::searchBv(PredictionUnit& pu, int xPos, int yPos, int width, int height, int picWidth, int picHeight, int xBv, int yBv, int ctuSize)
{
  const int ctuSizeLog2 = floorLog2(ctuSize);

  int refRightX = xPos + xBv + width - 1;
  int refBottomY = yPos + yBv + height - 1;

  int refLeftX = xPos + xBv;
  int refTopY = yPos + yBv;

  if ((xPos + xBv) < 0)
  {
    return false;
  }
  if (refRightX >= picWidth)
  {
    return false;
  }

  if ((yPos + yBv) < 0)
  {
    return false;
  }
  if (refBottomY >= picHeight)
  {
    return false;
  }
  if ((xBv + width) > 0 && (yBv + height) > 0)
  {
    return false;
  }

  // Don't search the above CTU row
  if (refTopY >> ctuSizeLog2 < yPos >> ctuSizeLog2)
  {
    return false;
  }

  // Don't search the below CTU row
  if (refBottomY >> ctuSizeLog2 > yPos >> ctuSizeLog2)
  {
    return false;
  }

  unsigned curTileIdx = pu.cs->pps->getTileIdx(pu.lumaPos());
  unsigned refTileIdx = pu.cs->pps->getTileIdx(Position(refLeftX, refTopY));
  if (curTileIdx != refTileIdx)
  {
    return false;
  }
  refTileIdx = pu.cs->pps->getTileIdx(Position(refLeftX, refBottomY));
  if (curTileIdx != refTileIdx)
  {
    return false;
  }
  refTileIdx = pu.cs->pps->getTileIdx(Position(refRightX, refTopY));
  if (curTileIdx != refTileIdx)
  {
    return false;
  }
  refTileIdx = pu.cs->pps->getTileIdx(Position(refRightX, refBottomY));
  if (curTileIdx != refTileIdx)
  {
    return false;
  }

  // in the same CTU line
  int numLeftCTUs = (1 << ((7 - ctuSizeLog2) << 1)) - ((ctuSizeLog2 < 7) ? 1 : 0);
  if ((refRightX >> ctuSizeLog2 <= xPos >> ctuSizeLog2) && (refLeftX >> ctuSizeLog2 >= (xPos >> ctuSizeLog2) - numLeftCTUs))
  {

    // in the same CTU, or left CTU
    // if part of ref block is in the left CTU, some area can be referred from the not-yet updated local CTU buffer
    if (((refLeftX >> ctuSizeLog2) == ((xPos >> ctuSizeLog2) - 1)) && (ctuSizeLog2 == 7))
    {
      // ref block's collocated block in current CTU
      const Position refPosCol = pu.Y().topLeft().offset(xBv + ctuSize, yBv);
      int offset64x = (refPosCol.x >> (ctuSizeLog2 - 1)) << (ctuSizeLog2 - 1);
      int offset64y = (refPosCol.y >> (ctuSizeLog2 - 1)) << (ctuSizeLog2 - 1);
      const Position refPosCol64x64 = {offset64x, offset64y};
      if (pu.cs->isDecomp(refPosCol64x64, toChannelType(COMPONENT_Y)))
      {
        return false;
      }
      if (refPosCol64x64 == pu.Y().topLeft())
      {
        return false;
      }
    }
  }
  else
  {
    return false;
  }

  // in the same CTU, or valid area from left CTU. Check if the reference block is already coded
  const Position refPosLT = pu.Y().topLeft().offset(xBv, yBv);
  const Position refPosBR = pu.Y().bottomRight().offset(xBv, yBv);
  const ChannelType      chType = toChannelType(COMPONENT_Y);
  if (!pu.cs->isDecomp(refPosBR, chType))
  {
    return false;
  }
  if (!pu.cs->isDecomp(refPosLT, chType))
  {
    return false;
  }
  return true;
}

//! \}
