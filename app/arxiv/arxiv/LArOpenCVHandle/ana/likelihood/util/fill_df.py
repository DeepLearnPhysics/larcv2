import numpy as np
import pandas as pd
from larocv import larocv

def track_shower_assumption(df):
    df['trkid'] = df.apply(lambda x : 0 if(x['par1_type']==1) else 1,axis=1)
    df['shrid'] = df.apply(lambda x : 1 if(x['par2_type']==2) else 0,axis=1)
    
    df['trk_frac_avg'] = df.apply(lambda x : x['par1_frac'] if(x['par1_type']==1) else x['par2_frac'],axis=1)
    df['shr_frac_avg'] = df.apply(lambda x : x['par2_frac'] if(x['par2_type']==2) else x['par1_frac'],axis=1)


def nue_assumption(comb_cut_df):
    print "Asking nue assumption"
    print "Asking npar==2"
    print "Asking in_fiducial==1"
    print "Asking pathexists2==1"

    comb_cut_df = comb_cut_df.query("npar==2")
    track_shower_assumption(comb_cut_df)
    comb_cut_df = comb_cut_df.query("par1_type != par2_type")
    comb_cut_df = comb_cut_df.query("in_fiducial==1")
    comb_cut_df = comb_cut_df.query("pathexists2==1")

    return comb_cut_df


def fill_parameters(comb_cut_df) :
    # SSNet Fraction
    #
    comb_cut_df['trk_frac'] = comb_cut_df.apply(lambda x : x['trk_frac_avg'] / x['nplanes_v'][x['trkid']],axis=1) 
    comb_cut_df['shr_frac'] = comb_cut_df.apply(lambda x : x['shr_frac_avg'] / x['nplanes_v'][x['shrid']],axis=1) 
    
    #
    # PCA
    #
    
    comb_cut_df['cosangle3d']=comb_cut_df.apply(lambda x : larocv.CosOpeningAngle(x['par_trunk_pca_theta_estimate_v'][0],
                                                                                  x['par_trunk_pca_phi_estimate_v'][0],
                                                                                  x['par_trunk_pca_theta_estimate_v'][1],
                                                                                  x['par_trunk_pca_phi_estimate_v'][1]),axis=1)
    
    comb_cut_df['angle3d'] = comb_cut_df.apply(lambda x : np.arccos(x['cosangle3d']),axis=1)

    
    comb_cut_df['shr_trunk_pca_theta_estimate'] = comb_cut_df.apply(lambda x : x['par_trunk_pca_theta_estimate_v'][x['shrid']],axis=1) 
    comb_cut_df['trk_trunk_pca_theta_estimate'] = comb_cut_df.apply(lambda x : x['par_trunk_pca_theta_estimate_v'][x['trkid']],axis=1) 
    
    comb_cut_df['shr_trunk_pca_cos_theta_estimate'] = comb_cut_df.apply(lambda x : np.cos(x['par_trunk_pca_theta_estimate_v'][x['shrid']]),axis=1) 
    comb_cut_df['trk_trunk_pca_cos_theta_estimate'] = comb_cut_df.apply(lambda x : np.cos(x['par_trunk_pca_theta_estimate_v'][x['trkid']]),axis=1) 
    
    
    #
    # 3D
    #
    comb_cut_df['shr_3d_length'] = comb_cut_df.apply(lambda x : x['par_pca_end_len_v'][x['shrid']],axis=1)
    comb_cut_df['trk_3d_length'] = comb_cut_df.apply(lambda x : x['par_pca_end_len_v'][x['trkid']],axis=1)
    
    comb_cut_df['shr_3d_QavgL'] = comb_cut_df.apply(lambda x : x['qsum_v'][x['shrid']] / x['par_pca_end_len_v'][x['shrid']] / x['nplanes_v'][x['shrid']],axis=1)
    comb_cut_df['trk_3d_QavgL'] = comb_cut_df.apply(lambda x : x['qsum_v'][x['trkid']] / x['par_pca_end_len_v'][x['trkid']] / x['nplanes_v'][x['trkid']],axis=1)
    
    #
    # Max deflection
    #
    comb_cut_df['shr_triangle_d_max'] = comb_cut_df.apply(lambda x : x['triangle_d_max_v'][x['shrid']],axis=1)
    comb_cut_df['trk_triangle_d_max'] = comb_cut_df.apply(lambda x : x['triangle_d_max_v'][x['trkid']],axis=1)
    
    #
    # Mean pixel dist from 2D PCA
    #
    comb_cut_df['shr_mean_pixel_dist'] = comb_cut_df.apply(lambda x : x['mean_pixel_dist_v'][x['shrid']]/x['nplanes_v'][x['shrid']],axis=1)
    comb_cut_df['shr_mean_pixel_dist_max'] = comb_cut_df.apply(lambda x : x['mean_pixel_dist_max_v'][x['shrid']],axis=1)
    comb_cut_df['shr_mean_pixel_dist_min'] = comb_cut_df.apply(lambda x : x['mean_pixel_dist_min_v'][x['shrid']],axis=1)
    comb_cut_df['shr_mean_pixel_dist_ratio'] = comb_cut_df.apply(lambda x : x['mean_pixel_dist_min_v'][x['shrid']] / x['mean_pixel_dist_max_v'][x['shrid']],axis=1)
    
    comb_cut_df['trk_mean_pixel_dist'] = comb_cut_df.apply(lambda x : x['mean_pixel_dist_v'][x['trkid']]/x['nplanes_v'][x['trkid']],axis=1)
    comb_cut_df['trk_mean_pixel_dist_max'] = comb_cut_df.apply(lambda x : x['mean_pixel_dist_max_v'][x['trkid']],axis=1)
    comb_cut_df['trk_mean_pixel_dist_min'] = comb_cut_df.apply(lambda x : x['mean_pixel_dist_min_v'][x['trkid']],axis=1)
    comb_cut_df['trk_mean_pixel_dist_ratio'] = comb_cut_df.apply(lambda x : x['mean_pixel_dist_min_v'][x['trkid']] / x['mean_pixel_dist_max_v'][x['trkid']],axis=1)     
    
    #
    # Sigma pixel dist from 2D PCA
    #
    comb_cut_df['shr_sigma_pixel_dist']       = comb_cut_df.apply(lambda x : x['sigma_pixel_dist_v'][x['shrid']]/x['nplanes_v'][x['shrid']],axis=1)
    comb_cut_df['shr_sigma_pixel_dist_max']   = comb_cut_df.apply(lambda x : x['sigma_pixel_dist_max_v'][x['shrid']],axis=1)
    comb_cut_df['shr_sigma_pixel_dist_min']   = comb_cut_df.apply(lambda x : x['sigma_pixel_dist_min_v'][x['shrid']],axis=1)
    comb_cut_df['shr_sigma_pixel_dist_ratio'] = comb_cut_df.apply(lambda x : x['sigma_pixel_dist_min_v'][x['shrid']] / x['sigma_pixel_dist_max_v'][x['shrid']],axis=1)
    
    comb_cut_df['trk_sigma_pixel_dist']       = comb_cut_df.apply(lambda x : x['sigma_pixel_dist_v'][x['trkid']]/x['nplanes_v'][x['trkid']],axis=1)
    comb_cut_df['trk_sigma_pixel_dist_max']   = comb_cut_df.apply(lambda x : x['sigma_pixel_dist_max_v'][x['trkid']],axis=1)
    comb_cut_df['trk_sigma_pixel_dist_min']   = comb_cut_df.apply(lambda x : x['sigma_pixel_dist_min_v'][x['trkid']],axis=1)
    comb_cut_df['trk_sigma_pixel_dist_ratio'] = comb_cut_df.apply(lambda x : x['sigma_pixel_dist_min_v'][x['trkid']] / x['sigma_pixel_dist_max_v'][x['trkid']],axis=1)    
    
    #
    # Ratio of # num pixels
    #
    comb_cut_df['shr_par_pixel_ratio'] = comb_cut_df.apply(lambda x : x['par_pixel_ratio_v'][x['shrid']],axis=1)
    comb_cut_df['trk_par_pixel_ratio'] = comb_cut_df.apply(lambda x : x['par_pixel_ratio_v'][x['trkid']],axis=1) 
    
    #
    # 2D angle difference @ vertex
    #
    comb_cut_df['anglediff0'] = comb_cut_df['anglediff'].values 
    
    #
    # 2D length
    #
    comb_cut_df['shr_avg_length']   = comb_cut_df.apply(lambda x : x['length_v'][x['shrid']] / x['nplanes_v'][x['shrid']],axis=1)
    comb_cut_df['shr_length_min']   = comb_cut_df.apply(lambda x : x['length_min_v'][x['shrid']],axis=1)
    comb_cut_df['shr_length_max']   = comb_cut_df.apply(lambda x : x['length_max_v'][x['shrid']],axis=1)
    comb_cut_df['shr_length_ratio'] = comb_cut_df.apply(lambda x : x['length_min_v'][x['shrid']] / x['length_max_v'][x['shrid']],axis=1)
    
    comb_cut_df['trk_avg_length']   = comb_cut_df.apply(lambda x : x['length_v'][x['trkid']] / x['nplanes_v'][x['trkid']],axis=1)
    comb_cut_df['trk_length_min']   = comb_cut_df.apply(lambda x : x['length_min_v'][x['trkid']],axis=1)
    comb_cut_df['trk_length_max']   = comb_cut_df.apply(lambda x : x['length_max_v'][x['trkid']],axis=1)
    comb_cut_df['trk_length_ratio'] = comb_cut_df.apply(lambda x : x['length_min_v'][x['trkid']] / x['length_max_v'][x['trkid']],axis=1)
    
    #
    # 2D width
    #
    comb_cut_df['shr_avg_width']   = comb_cut_df.apply(lambda x : x['width_v'][x['shrid']] / x['nplanes_v'][x['shrid']],axis=1)
    comb_cut_df['shr_width_min']   = comb_cut_df.apply(lambda x : x['width_min_v'][x['shrid']],axis=1)
    comb_cut_df['shr_width_max']   = comb_cut_df.apply(lambda x : x['width_max_v'][x['shrid']],axis=1)
    comb_cut_df['shr_width_ratio'] = comb_cut_df.apply(lambda x : x['width_min_v'][x['shrid']] / x['width_max_v'][x['shrid']],axis=1)
    
    comb_cut_df['trk_avg_width']   = comb_cut_df.apply(lambda x : x['width_v'][x['trkid']] / x['nplanes_v'][x['trkid']],axis=1)
    comb_cut_df['trk_width_max']   = comb_cut_df.apply(lambda x : x['width_max_v'][x['trkid']],axis=1)
    comb_cut_df['trk_width_min']   = comb_cut_df.apply(lambda x : x['width_max_v'][x['trkid']],axis=1)
    comb_cut_df['trk_width_ratio'] = comb_cut_df.apply(lambda x : x['width_min_v'][x['trkid']] / x['width_max_v'][x['trkid']],axis=1)
    
    #
    # 2D perimeter
    #
    comb_cut_df['shr_avg_perimeter'] = comb_cut_df.apply(lambda x : x['perimeter_v'][x['shrid']] / x['nplanes_v'][x['shrid']],axis=1)
    comb_cut_df['shr_perimeter_min'] = comb_cut_df.apply(lambda x : x['perimeter_min_v'][x['shrid']],axis=1)
    comb_cut_df['shr_perimeter_max'] = comb_cut_df.apply(lambda x : x['perimeter_max_v'][x['shrid']],axis=1)
    comb_cut_df['shr_perimeter_ratio'] = comb_cut_df.apply(lambda x : x['perimeter_min_v'][x['shrid']] / x['perimeter_max_v'][x['shrid']],axis=1)
    
    comb_cut_df['trk_avg_perimeter'] = comb_cut_df.apply(lambda x : x['perimeter_v'][x['trkid']] / x['nplanes_v'][x['trkid']],axis=1)
    comb_cut_df['trk_perimeter_min'] = comb_cut_df.apply(lambda x : x['perimeter_max_v'][x['trkid']],axis=1)
    comb_cut_df['trk_perimeter_max'] = comb_cut_df.apply(lambda x : x['perimeter_max_v'][x['trkid']],axis=1)
    comb_cut_df['trk_perimeter_ratio'] = comb_cut_df.apply(lambda x : x['perimeter_min_v'][x['trkid']] / x['perimeter_max_v'][x['trkid']],axis=1)
    
    #
    # 2D area
    #
    comb_cut_df['shr_avg_area'] = comb_cut_df.apply(lambda x : x['area_v'][x['shrid']] / x['nplanes_v'][x['shrid']],axis=1)
    comb_cut_df['shr_area_min'] = comb_cut_df.apply(lambda x : x['area_min_v'][x['shrid']],axis=1)
    comb_cut_df['shr_area_max'] = comb_cut_df.apply(lambda x : x['area_max_v'][x['shrid']],axis=1)
    comb_cut_df['shr_area_ratio'] = comb_cut_df.apply(lambda x : x['area_min_v'][x['shrid']] / x['area_max_v'][x['shrid']],axis=1)
    
    comb_cut_df['trk_avg_area'] = comb_cut_df.apply(lambda x : x['area_v'][x['trkid']] / x['nplanes_v'][x['trkid']],axis=1)
    comb_cut_df['trk_area_min'] = comb_cut_df.apply(lambda x : x['area_max_v'][x['trkid']],axis=1)
    comb_cut_df['trk_area_max'] = comb_cut_df.apply(lambda x : x['area_max_v'][x['trkid']],axis=1)
    comb_cut_df['trk_area_ratio'] = comb_cut_df.apply(lambda x : x['area_min_v'][x['trkid']] / x['area_max_v'][x['trkid']],axis=1)
    
    #
    # N pixel
    #
    comb_cut_df['shr_avg_npixel'] = comb_cut_df.apply(lambda x : x['npixel_v'][x['shrid']] / x['nplanes_v'][x['shrid']],axis=1)
    comb_cut_df['shr_npixel_min'] = comb_cut_df.apply(lambda x : x['npixel_min_v'][x['shrid']],axis=1)
    comb_cut_df['shr_npixel_max'] = comb_cut_df.apply(lambda x : x['npixel_max_v'][x['shrid']],axis=1)
    comb_cut_df['shr_npixel_ratio'] = comb_cut_df.apply(lambda x : x['npixel_min_v'][x['shrid']] / x['npixel_max_v'][x['shrid']],axis=1)
    
    comb_cut_df['trk_avg_npixel'] = comb_cut_df.apply(lambda x : x['npixel_v'][x['trkid']] / x['nplanes_v'][x['trkid']],axis=1)
    comb_cut_df['trk_npixel_min'] = comb_cut_df.apply(lambda x : x['npixel_max_v'][x['trkid']],axis=1)
    comb_cut_df['trk_npixel_max'] = comb_cut_df.apply(lambda x : x['npixel_max_v'][x['trkid']],axis=1)
    comb_cut_df['trk_npixel_ratio'] = comb_cut_df.apply(lambda x : x['npixel_min_v'][x['trkid']] / x['npixel_max_v'][x['trkid']],axis=1)
    
    #
    # Q sum
    #
    comb_cut_df['shr_avg_qsum']   = comb_cut_df.apply(lambda x : x['qsum_v'][x['shrid']] / x['nplanes_v'][x['shrid']],axis=1)
    comb_cut_df['shr_qsum_min']   = comb_cut_df.apply(lambda x : x['qsum_min_v'][x['shrid']],axis=1)
    comb_cut_df['shr_qsum_max']   = comb_cut_df.apply(lambda x : x['qsum_max_v'][x['shrid']],axis=1)
    comb_cut_df['shr_qsum_ratio'] = comb_cut_df.apply(lambda x : x['qsum_min_v'][x['shrid']] / x['qsum_max_v'][x['shrid']],axis=1)
    
    comb_cut_df['trk_avg_qsum']   = comb_cut_df.apply(lambda x : x['qsum_v'][x['trkid']] / x['nplanes_v'][x['trkid']],axis=1)
    comb_cut_df['trk_qsum_min']   = comb_cut_df.apply(lambda x : x['qsum_max_v'][x['trkid']],axis=1)
    comb_cut_df['trk_qsum_max']   = comb_cut_df.apply(lambda x : x['qsum_max_v'][x['trkid']],axis=1)
    comb_cut_df['trk_qsum_ratio'] = comb_cut_df.apply(lambda x : x['qsum_min_v'][x['trkid']] / x['qsum_max_v'][x['trkid']],axis=1)
    
    return comb_cut_df
