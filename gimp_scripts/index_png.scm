; Important! inInputImagesPattern must have
; \\ file separators on windows and / separators on unix.
(define (script-fu-tractor-converter-index-png inInputImagesPattern
                                               inOutputFolder
                                               inDitheting)
   (let*
      (
         ; Layers have their own resolution so image resolution is not used.
         ; 340x255 is resolution of item preview *.avi.
;         (theAviWidth            340)
;         (theAviHeight           255)

         (TheMaxPalColorsNum     256)
         (thePalColorsNum        127)
         (thePalColorsToSkipNum  128)



         (theInImagesFilelist (cadr (file-glob inInputImagesPattern 1)))


         ; Layers have their own resolution so image resolution is not used.
         (theImageToIndex (car (gimp-image-new 1   ; theAviWidth
                                               1   ; theAviHeight
                                               RGB
                               )
                          )
         )
      )

      (while (not (null? theInImagesFilelist))
         (let*
            (
               (theFilename (car theInImagesFilelist))
               (theLayer (car (gimp-file-load-layer RUN-NONINTERACTIVE
                                                    theImageToIndex
                                                    theFilename
                              )
                         )
               )
            )

            ; TEST
            (gimp-message (string-append "Loading: " theFilename))

            (gimp-image-insert-layer theImageToIndex
                                     theLayer
                                     0
                                     -1
            )

            (gimp-image-set-active-layer theImageToIndex
                                         theLayer
            )
         )
         (set! theInImagesFilelist (cdr theInImagesFilelist))
      )



      (gimp-image-convert-indexed theImageToIndex
                                  inDitheting     ; Dithering. Turned off.
                                  MAKE-PALETTE
                                  thePalColorsNum
                                  FALSE
                                  FALSE           ; ignored
                                  ""              ; ignored
      )



      ; Expanding number of colors in palette to 256.
      ; New colors are zeros.
      (let*
         (
            (theGeneratedColorMapInfo
               (gimp-image-get-colormap theImageToIndex)
            )
            (theGeneratedNumBytes (car theGeneratedColorMapInfo))
            (theGeneratedColorMap (cadr theGeneratedColorMapInfo))
            (theCurByteToCopy 0)
            (theCurByteValue 0)
            (theMaxColorMapBytesNum 768)
            (theColorMapArr (make-vector theMaxColorMapBytesNum 0))
         )

         ; Shifting by 128 colors.
         (while (< theCurByteToCopy theGeneratedNumBytes)

            (set! theCurByteValue (vector-ref theGeneratedColorMap
                                              theCurByteToCopy
                                  )
            )

            (vector-set! theColorMapArr theCurByteToCopy theCurByteValue)

            (set! theCurByteToCopy (+ theCurByteToCopy 1))
         )

         (gimp-image-set-colormap theImageToIndex
                                  theMaxColorMapBytesNum
                                  theColorMapArr
         )
      )



      ; Remapping the palette.
      ; Remap should shift palette right by thePalColorsToSkipNum.
      (let*
         (
            (theRemapMap (make-vector TheMaxPalColorsNum 0))
            (theCurSourceColor 0)
            (theCurEndColor thePalColorsToSkipNum)
         )

         ; Filling colors of result palette in range
         ; thePalColorsToSkipNum - TheMaxPalColorsNum
         ; with first colors of source palette.
         (while (< theCurEndColor TheMaxPalColorsNum)
            (vector-set! theRemapMap theCurEndColor theCurSourceColor)
            (set! theCurSourceColor (+ theCurSourceColor 1))
            (set! theCurEndColor    (+ theCurEndColor    1))
         )

         (set! theCurEndColor 0)

         ; Filling colors of result palette in range
         ; 0 - thePalColorsToSkipNum
         ; with subsequent colors of source palette.
         (while (< theCurEndColor thePalColorsToSkipNum)
            (vector-set! theRemapMap theCurEndColor theCurSourceColor)
            (set! theCurSourceColor (+ theCurSourceColor 1))
            (set! theCurEndColor    (+ theCurEndColor    1))
         )

         (plug-in-colormap-remap RUN-NONINTERACTIVE
                                 theImageToIndex
                                 -1                 ; Drawable. Never used.
                                 TheMaxPalColorsNum
                                 theRemapMap
         )
      )



      (let*
         (
            (theLayersInfo (gimp-image-get-layers theImageToIndex))
            (theNumLayers (car theLayersInfo))
            (theLayerArray (cadr theLayersInfo))
            (theCurLayerInd 0)
            (theCurLayerID -1)
            (theCurLayerName "")
         )
         (while (< theCurLayerInd theNumLayers)
            (set! theCurLayerID (vector-ref theLayerArray theCurLayerInd))
            (set! theCurLayerName (car (gimp-layer-get-name theCurLayerID)))
            
            ; TEST
            (gimp-message (string-append "Saving: "
                                         inOutputFolder
                                         "/"
                                         theCurLayerName
                          )
            )

            (file-png-save RUN-NONINTERACTIVE
                           theImageToIndex
                           theCurLayerID
                           (string-append inOutputFolder "/" theCurLayerName)
                           (string-append inOutputFolder "/" theCurLayerName)
                           0 ; Use Adam7 interlacing?
                           9 ; Deflate Compression factor (0--9).
                           0 ; Write bKGD chunk?
                           0 ; Write gAMA chunk?
                           0 ; Write oFFs chunk?
                           0 ; Write pHYs chunk?
                           0 ; Write tIME chunk?
            )

            (set! theCurLayerInd (+ theCurLayerInd 1))
         )
      )



      (gimp-image-delete theImageToIndex)
   )
)
