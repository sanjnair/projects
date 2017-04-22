package it.jaschke.alexandria.vision;

import com.google.android.gms.vision.MultiProcessor;
import com.google.android.gms.vision.Tracker;
import com.google.android.gms.vision.barcode.Barcode;

/**
 * Factory for creating a tracker and associated graphic to be associated with a new barcode.  The
 * multi-processor uses this factory to create barcode trackers as needed -- one for each barcode.
 */
public class BarcodeTrackerFactory implements MultiProcessor.Factory<Barcode> {
    private GraphicOverlay mGraphicOverlay;
    private BarcodeCallback barcodeCallback;

    public interface BarcodeCallback {
        void onBarCodeDetected(Barcode barcode);
    }

    public BarcodeTrackerFactory(GraphicOverlay graphicOverlay, BarcodeCallback barcodeCallback) {
        mGraphicOverlay = graphicOverlay;
        this.barcodeCallback = barcodeCallback;
    }

    @Override
    public Tracker<Barcode> create(Barcode barcode) {
        BarcodeGraphic graphic = new BarcodeGraphic(mGraphicOverlay, barcodeCallback);
        return new GraphicTracker<>(mGraphicOverlay, graphic);
    }
}