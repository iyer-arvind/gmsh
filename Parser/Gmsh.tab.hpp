typedef union {
  char *c;
  int i;
  unsigned int u;
  double d;
  double v[5];
  Shape s;
  List_T *l;
} YYSTYPE;
#define	tDOUBLE	257
#define	tSTRING	258
#define	tBIGSTR	259
#define	tEND	260
#define	tAFFECT	261
#define	tDOTS	262
#define	tPi	263
#define	tMPI_Rank	264
#define	tMPI_Size	265
#define	tExp	266
#define	tLog	267
#define	tLog10	268
#define	tSqrt	269
#define	tSin	270
#define	tAsin	271
#define	tCos	272
#define	tAcos	273
#define	tTan	274
#define	tRand	275
#define	tAtan	276
#define	tAtan2	277
#define	tSinh	278
#define	tCosh	279
#define	tTanh	280
#define	tFabs	281
#define	tFloor	282
#define	tCeil	283
#define	tFmod	284
#define	tModulo	285
#define	tHypot	286
#define	tPrintf	287
#define	tSprintf	288
#define	tStrCat	289
#define	tStrPrefix	290
#define	tStrRelative	291
#define	tBoundingBox	292
#define	tDraw	293
#define	tToday	294
#define	tPoint	295
#define	tCircle	296
#define	tEllipse	297
#define	tLine	298
#define	tSphere	299
#define	tSurface	300
#define	tSpline	301
#define	tVolume	302
#define	tCharacteristic	303
#define	tLength	304
#define	tParametric	305
#define	tElliptic	306
#define	tPlane	307
#define	tRuled	308
#define	tTransfinite	309
#define	tComplex	310
#define	tPhysical	311
#define	tUsing	312
#define	tBump	313
#define	tProgression	314
#define	tPlugin	315
#define	tRotate	316
#define	tTranslate	317
#define	tSymmetry	318
#define	tDilate	319
#define	tExtrude	320
#define	tDuplicata	321
#define	tLoop	322
#define	tRecombine	323
#define	tDelete	324
#define	tCoherence	325
#define	tAttractor	326
#define	tLayers	327
#define	tAlias	328
#define	tAliasWithOptions	329
#define	tText2D	330
#define	tText3D	331
#define	tInterpolationScheme	332
#define	tTime	333
#define	tGrain	334
#define	tCombine	335
#define	tBSpline	336
#define	tBezier	337
#define	tNurbs	338
#define	tOrder	339
#define	tKnots	340
#define	tColor	341
#define	tColorTable	342
#define	tFor	343
#define	tIn	344
#define	tEndFor	345
#define	tIf	346
#define	tEndIf	347
#define	tExit	348
#define	tReturn	349
#define	tCall	350
#define	tFunction	351
#define	tShow	352
#define	tHide	353
#define	tGetValue	354
#define	tGMSH_MAJOR_VERSION	355
#define	tGMSH_MINOR_VERSION	356
#define	tGMSH_PATCH_VERSION	357
#define	tAFFECTPLUS	358
#define	tAFFECTMINUS	359
#define	tAFFECTTIMES	360
#define	tAFFECTDIVIDE	361
#define	tOR	362
#define	tAND	363
#define	tEQUAL	364
#define	tNOTEQUAL	365
#define	tLESSOREQUAL	366
#define	tGREATEROREQUAL	367
#define	tPLUSPLUS	368
#define	tMINUSMINUS	369
#define	UNARYPREC	370


extern YYSTYPE yylval;
