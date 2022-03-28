#include "g4fire/geo/AuxInfoReader.h"

#include <stdlib.h>
#include <string>

#include "G4FieldManager.hh"
#include "G4GDMLEvaluator.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UniformMagField.hh"

//#include "Framework/Exception/Exception.h"
#include "fire/exception/Exception.h"

/*#include "g4fire/CalorimeterSD.h"
#include "g4fire/EcalSD.h"
#include "g4fire/HcalSD.h"*/
#include "g4fire/MagneticFieldMap3D.h"
#include "g4fire/MagneticFieldStore.h"
/*#include "g4fire/ScoringPlaneSD.h"
#include "g4fire/TrackerSD.h"
#include "g4fire/TrigScintSD.h"*/
#include "g4fire/UserRegionInformation.h"
#include "g4fire/VisAttributesStore.h"

namespace g4fire::geo {

AuxInfoReader::AuxInfoReader(G4GDMLParser *parser, fire::config::Parameters ps,
                             ConditionsInterface &ci)
    : parser_(parser), params_(ps), conditions_intf_(ci) {

  eval_ = std::make_unique<G4GDMLEvaluator>();
}

// AuxInfoReader::~AuxInfoReader() {
// delete detectorHeader_;
//}

void AuxInfoReader::readGlobalAuxInfo() {
  auto aux_info_list{parser_->GetAuxList()};
  std::cout << "Got aux list." << std::endl;
  for (std::vector<G4GDMLAuxStructType>::const_iterator iaux =
           aux_info_list->begin();
       iaux != aux_info_list->end(); iaux++) {
    G4String aux_type = iaux->type;
    G4String aux_val = iaux->value;
    G4String aux_unit = iaux->unit;
    std::cout << "aux type: " << aux_type << std::endl;
    std::cout << "aux val: " << aux_val << std::endl;
    std::cout << "aux unit: " << aux_unit << std::endl;
    if (aux_type == "SensDet") {
      createSensitiveDetector(aux_val, iaux->auxList);
    } else if (aux_type == "MagneticField") {
      createMagneticField(aux_val, iaux->auxList);
    } else if (aux_type == "Region") {
      createRegion(aux_val, iaux->auxList);
    } else if (aux_type == "VisAttributes") {
      createVisAttributes(aux_val, iaux->auxList);
    } /*else if (aux_type == "DetectorVersion") {
      createDetectorHeader(aux_val, iaux->auxList);
    }*/
  }
  return;
}

void AuxInfoReader::createSensitiveDetector(
    G4String sd_name, const G4GDMLAuxListType *aux_info_list) {
  // G4cout << "Creating SensitiveDetector " << sd_name << G4endl;

  G4String sd_type("");
  G4String hc_name("");
  G4String id_name("");
  int subdet_id = -1;
  int layer_depth = -1;
  int verbose = 0;
  for (std::vector<G4GDMLAuxStructType>::const_iterator iaux =
           aux_info_list->begin();
       iaux != aux_info_list->end(); iaux++) {
    G4String aux_type = iaux->type;
    G4String aux_val = iaux->value;
    G4String aux_unit = iaux->unit;

    // G4cout << "aux_type: " << aux_type << ", aux_val: " << aux_val << ",
    // aux_unit: " << aux_unit << G4endl;

    if (aux_type == "SensDetType") {
      sd_type = aux_val;
    } else if (aux_type == "HitsCollection") {
      hc_name = aux_val;
    } else if (aux_type == "Verbose") {
      verbose = atoi(aux_val.c_str());
    } else if (aux_type == "SubdetID") {
      subdet_id = atoi(aux_val.c_str());
    } else if (aux_type == "DetectorID") {
      id_name = aux_val;
    } else if (aux_type == "LayerDepth") {
      layer_depth = atoi(aux_val.c_str());
    }
  }

  if (sd_type == "") {
    // EXCEPTION_RAISE("MissingInfo", "The SensDet is missing the
    // SensDetType.");
  }

  if (hc_name == "") {
    // EXCEPTION_RAISE("MissingInfo",
    //                "The SensDet is missing the HitsCollection.");
  }

  if (subdet_id <= 0) {
    // EXCEPTION_RAISE("BadID", "The SubdetID '" + std::to_string(subdet_id) +
    //                             "' is missing or invalid.");
  }

  /*
   * Build the Sensitive Detector, and re-assign the detID if applicable
   */
  // G4VSensitiveDetector *sd = 0;

  /*
  if (sd_type == "TrackerSD") {
    sd = new TrackerSD(sd_name, hc_name, subdet_id);
  } else if (sd_type == "EcalSD") {
    sd = new EcalSD(sd_name, hc_name, subdet_id, conditions_intf_);
  } else if (sd_type == "HcalSD") {
    sd = new HcalSD(sd_name, hc_name, subdet_id);
  } else if (sd_type == "ScoringPlaneSD") {
    sd = new ScoringPlaneSD(sd_name, hc_name, subdet_id);
  } else if (sd_type == "TrigScintSD") {
    sd = new TrigScintSD(sd_name, hc_name, subdet_id);
  } else {
    // EXCEPTION_RAISE("DetType", "Unknown SensitiveDetector type: " + sd_type);
  }*/

  /*
   * Fix  layer depth if the Sensitive Detector is not the Tracker
   */
  /*if (sd_type != "TrackerSD" && layer_depth != -1) {
    ((CalorimeterSD *)sd)->setLayerDepth(layer_depth);
  }
  sd->SetVerboseLevel(verbose);*/

  // G4cout << "Created " << sd_type << " " << sd_name << " with hits
  // collection " << hc_name << " and verbose level " << verbose << G4endl <<
  // G4endl;
}

void AuxInfoReader::assignAuxInfoToVolumes() {
  const G4LogicalVolumeStore *lvs = G4LogicalVolumeStore::GetInstance();
  std::vector<G4LogicalVolume *>::const_iterator lvciter;
  for (lvciter = lvs->begin(); lvciter != lvs->end(); lvciter++) {
    G4GDMLAuxListType aux_info =
        parser_->GetVolumeAuxiliaryInformation(*lvciter);
    if (aux_info.size() > 0) {
      for (std::vector<G4GDMLAuxStructType>::const_iterator iaux =
               aux_info.begin();
           iaux != aux_info.end(); iaux++) {
        G4String aux_type = iaux->type;
        G4String aux_val = iaux->value;
        G4String aux_unit = iaux->unit;

        G4LogicalVolume *lv = (*lvciter);

        if (aux_type == "SensDet") {
          G4String sd_name = aux_val;
          G4VSensitiveDetector *sd =
              G4SDManager::GetSDMpointer()->FindSensitiveDetector(sd_name);
          if (sd != NULL) {
            lv->SetSensitiveDetector(sd);
            // G4cout << "Assigned SD " << sd->GetName() << " to " <<
            // lv->GetName() << G4endl;
          } else {
            throw fire::Exception(
                "MissingInfo",
                "Unknown SensDet in volume's auxiliary info: " +
                    std::string(sd_name.data()),
                false);
          }
        } else if (aux_type == "MagneticField") {
          G4String mag_field_name = aux_val;
          G4MagneticField *mag_field =
              MagneticFieldStore::getInstance()->getMagneticField(
                  mag_field_name);
          if (mag_field != NULL) {
            G4FieldManager *mgr = new G4FieldManager(mag_field);
            lv->SetFieldManager(mgr, true /* FIXME: hard-coded to force field manager to daughters */);
            // G4cout << "Assigned magnetic field " << mag_field_name << " to
            // volume " << lv->GetName() << G4endl;
          } else {
            throw fire::Exception(
                "MissingInfo",
                "Unknown MagneticField ref in volume's auxiliary info: " +
                    std::string(mag_field_name.data()),
                false);
          }
        } else if (aux_type == "Region") {
          G4String region_name = aux_val;
          G4Region *region =
              G4RegionStore::GetInstance()->GetRegion(region_name);
          if (region != NULL) {
            region->AddRootLogicalVolume(lv);
            // G4cout << "Added volume " << lv->GetName() << " to region " <<
            // region_name << G4endl;
          } else {
            throw fire::Exception("MissingInfo",
                                  "Reference region '" +
                                      std::string(region_name.data()) +
                                      "' was not found!",
                                  false);
          }
        } else if (aux_type == "VisAttributes") {
          G4String vis_name = aux_val;
          G4VisAttributes *visAttributes =
              VisAttributesStore::getInstance()->getVisAttributes(vis_name);
          if (visAttributes != NULL) {
            lv->SetVisAttributes(visAttributes);
            // G4cout << "Assigned VisAttributes " << vis_name << " to volume "
            // << lv->GetName() << G4endl;
          } else {
            throw fire::Exception("MissingInfo",
                                  "Referenced VisAttributes '" +
                                      std::string(vis_name.data()) +
                                      "' was not found!",
                                  false);
          }
        }
      }
    }
  }
}

void AuxInfoReader::createMagneticField(
    G4String mag_field_name, const G4GDMLAuxListType *aux_info_list) {
  // Find type of the mag field.
  G4String mag_field_type("");
  for (std::vector<G4GDMLAuxStructType>::const_iterator iaux =
           aux_info_list->begin();
       iaux != aux_info_list->end(); iaux++) {
    G4String aux_type = iaux->type;
    G4String aux_val = iaux->value;

    if (aux_type == "MagneticFieldType") {
      mag_field_type = aux_val;
      break;
    }
  }

  if (mag_field_type == "") {
    throw fire::Exception("MissingInfo",
                          "Missing MagFieldType for magnetic field definition.",
                          false);
  }

  G4MagneticField *mag_field = NULL;

  // Create a uniform mag field using the built-in Geant4 type.
  if (mag_field_type == "G4UniformMagField") {
    std::string::size_type sz;
    double bx, by, bz;
    bx = by = bz = 0.;
    for (std::vector<G4GDMLAuxStructType>::const_iterator iaux =
             aux_info_list->begin();
         iaux != aux_info_list->end(); iaux++) {
      G4String aux_type = iaux->type;
      G4String aux_val = iaux->value;
      G4String aux_unit = iaux->unit;

      G4String expr = aux_val + "*" + aux_unit;
      if (aux_type == "bx") {
        bx = eval_->Evaluate(expr);
      } else if (aux_type == "by") {
        by = eval_->Evaluate(expr);
      } else if (aux_type == "bz") {
        bz = eval_->Evaluate(expr);
      }
    }
    G4ThreeVector fieldComponents(bx, by, bz);
    mag_field = new G4UniformMagField(fieldComponents);

    // G4cout << "Created G4UniformMagField " << mag_field_name << " with field
    // components " << fieldComponents << G4endl << G4endl;

    // Create a global 3D field map by reading from a data file.
  } else if (mag_field_type == "MagneticFieldMap3D") {
    std::string file_name;
    double offset_x, offset_y, offset_z;

    for (std::vector<G4GDMLAuxStructType>::const_iterator iaux =
             aux_info_list->begin();
         iaux != aux_info_list->end(); iaux++) {
      G4String aux_type = iaux->type;
      G4String aux_val = iaux->value;
      G4String aux_unit = iaux->unit;

      G4String expr = aux_val + "*" + aux_unit;

      if (aux_type == "File") {
        file_name = aux_val;
      } else if (aux_type == "OffsetX") {
        offset_x = eval_->Evaluate(expr);
      } else if (aux_type == "OffsetY") {
        offset_y = eval_->Evaluate(expr);
      } else if (aux_type == "OffsetZ") {
        offset_z = eval_->Evaluate(expr);
      }
    }

    if (file_name.size() == 0) {
      throw fire::Exception(
          "MissingInfo", "File info with field data was not provided.", false);
    }

    // Create new 3D field map.
    mag_field =
        new MagneticFieldMap3D(file_name.c_str(), offset_x, offset_y, offset_z);

    // Assign field map as global field.
    G4FieldManager *field_mgr =
        G4TransportationManager::GetTransportationManager()->GetFieldManager();
    if (field_mgr->GetDetectorField() != nullptr) {
      throw fire::Exception("MisAssign",
                            "Global mag field was already assigned.", false);
    }
    field_mgr->SetDetectorField(mag_field);
    field_mgr->CreateChordFinder(mag_field);

  } else {
    throw fire::Exception("UnknownType",
                          "Unknown MagFieldType '" +
                              std::string(mag_field_type.data()) +
                              "' in auxiliary info.",
                          false);
  }

  MagneticFieldStore::getInstance()->addMagneticField(mag_field_name,
                                                      mag_field);
}

void AuxInfoReader::createRegion(G4String name,
                                 const G4GDMLAuxListType *aux_info_list) {
  bool store_trajectories = true;
  for (std::vector<G4GDMLAuxStructType>::const_iterator iaux =
           aux_info_list->begin();
       iaux != aux_info_list->end(); iaux++) {
    G4String aux_type = iaux->type;
    G4String aux_val = iaux->value;
    G4String aux_unit = iaux->unit;

    if (aux_type == "StoreTrajectories") {
      if (aux_val == "false") {
        store_trajectories = false;
      } else if (aux_val == "true") {
        store_trajectories = true;
      }
    }
  }

  G4VUserRegionInformation *region_info =
      new UserRegionInformation(store_trajectories);
  G4Region *region = new G4Region(name);
  region->SetUserInformation(region_info);

  // G4cout << "Created new detector region " << region->GetName() << G4endl <<
  // G4endl;
}

void AuxInfoReader::createVisAttributes(
    G4String name, const G4GDMLAuxListType *aux_info_list) {
  G4double rgba[4] = {1., 1., 1., 1.};
  G4bool visible = true;
  G4bool dau_invisible = false;
  G4bool force_wireframe = false;
  G4bool force_solid = false;
  G4double line_width = 1.0;
  G4VisAttributes::LineStyle line_style = G4VisAttributes::unbroken;

  for (std::vector<G4GDMLAuxStructType>::const_iterator iaux =
           aux_info_list->begin();
       iaux != aux_info_list->end(); iaux++) {
    G4String aux_type = iaux->type;
    G4String aux_val = iaux->value;
    G4String aux_unit = iaux->unit;

    if (aux_type == "R") {
      rgba[0] = atof(aux_val.c_str());
    } else if (aux_type == "G") {
      rgba[1] = atof(aux_val.c_str());
    } else if (aux_type == "B") {
      rgba[2] = atof(aux_val.c_str());
    } else if (aux_type == "A") {
      rgba[3] = atof(aux_val.c_str());
    } else if (aux_type == "Style") {
      if (aux_val == "wireframe") {
        force_wireframe = true;
      } else if (aux_val == "solid") {
        force_solid = true;
      }
    } else if (aux_type == "DaughtersInvisible") {
      if (aux_val == "true") {
        dau_invisible = true;
      } else if (aux_val == "false") {
        dau_invisible = false;
      }
    } else if (aux_type == "Visible") {
      if (aux_val == "true") {
        visible = true;
      } else if (aux_val == "false") {
        visible = false;
      }
    } else if (aux_type == "LineStyle") {
      if (aux_val == "unbroken") {
        line_style = G4VisAttributes::unbroken;
      } else if (aux_val == "dashed") {
        line_style = G4VisAttributes::dashed;
      } else if (aux_val == "dotted") {
        line_style = G4VisAttributes::dotted;
      }
    } else if (aux_type == "LineWidth") {
      line_width = atof(aux_val.c_str());
    }
  }

  G4VisAttributes *visAttributes = new G4VisAttributes();
  visAttributes->SetColor(rgba[0], rgba[1], rgba[2], rgba[3]);
  visAttributes->SetVisibility(visible);
  visAttributes->SetDaughtersInvisible(dau_invisible);
  visAttributes->SetForceWireframe(force_wireframe);
  visAttributes->SetForceSolid(force_solid);
  visAttributes->SetLineWidth(line_width);
  visAttributes->SetLineStyle(line_style);
  VisAttributesStore::getInstance()->addVisAttributes(name, visAttributes);

  // G4cout << "Created VisAttributes " << name << G4endl << (*visAttributes) <<
  // G4endl << G4endl;
}

void AuxInfoReader::createDetectorHeader(
    G4String aux_value, const G4GDMLAuxListType *aux_info_list) {
  int det_version = atoi(aux_value.c_str());

  std::string det_name("");
  std::string author("");
  std::string description("");

  for (std::vector<G4GDMLAuxStructType>::const_iterator iaux =
           aux_info_list->begin();
       iaux != aux_info_list->end(); iaux++) {
    G4String aux_type = iaux->type;
    G4String aux_val = iaux->value;
    G4String aux_unit = iaux->unit;

    if (aux_type == "DetectorName") {
      det_name = aux_val;
    } else if (aux_type == "Author") {
      author = aux_val;
    } else if (aux_type == "Description") {
      description = aux_val;
    }
  }

  // detectorHeader_ =
  //    new ldmx::DetectorHeader(det_name, det_version, description, author);

  /*G4cout << G4endl;
  G4cout << "Read detector header from userinfo: " << G4endl;
  G4cout << "  DetectorName: " << detectorHeader_->getName() << G4endl;
  G4cout << "  DetectorVersion: " << detectorHeader_->getVersion() << G4endl;
  G4cout << "  Author: " << detectorHeader_->getAuthor() << G4endl;
  G4cout << "  Description: " << detectorHeader_->getDescription() << G4endl;
  G4cout << G4endl;*/
}

} // namespace g4fire::geo
