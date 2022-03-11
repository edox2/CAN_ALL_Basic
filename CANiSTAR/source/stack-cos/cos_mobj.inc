   //--- Index 2000, Example function 1 -------------------
   {  0x2000, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED16       , (void *) &CosMob_Idx2000      },

   //--- Index 2001, Example function 2 -------------------
   {  0x2001, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED8        , (void *) &CosMob_Idx2001      },

   //--- Index 2002, Example variable 1 -------------------
   {  0x2002, 0x00, CoATTR_ACC_RW,
      CoDT_UNSIGNED8        , (void *) &ubCosMob_Var2002G   },

   //--- Index 2003, Example variable 2 -------------------
   {  0x2003, 0x00, CoATTR_ACC_RW,
      CoDT_UNSIGNED16       , (void *) &uwCosMob_Var2003G   },

   //--- Index 2004, Example variable 3 -------------------
   {  0x2004, 0x00, CoATTR_ACC_RW,
      CoDT_UNSIGNED32       , (void *) &ulCosMob_Var2004G   },

   //--- Index 2005, Example variable 4 -------------------
   #if CPP_DATA_SIZE >= 64
   {  0x2005, 0x00, CoATTR_ACC_RW,
      CoDT_UNSIGNED64       , (void *) &uqCosMob_Var2005G   },
   #endif

   //--- Index 2006, Example function 3 -------------------
   {  0x2006, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED16       , (void *) &CosMob_Idx2006      },

   //--- Index 2007, Example function 3 -------------------
   {  0x2007, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_DOMAIN           , (void *) &CosMob_Idx2007      },

   //--- Index 2008, Example string -----------------------
   {  0x2008, 0x00, CoATTR_ACC_RW,
      CoDT_VISIBLE_STRING   , (void *) &szCosMob_Str2008G   },

   #if COS_MOB_MC > 0
   #include "mc_co_mobj.inc"
   #endif





